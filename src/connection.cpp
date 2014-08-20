/**
 * This file is part of QFCgi.
 *
 * QFCgi is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * QFCgi is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with QFCgi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <QDebug>

#include "connection.h"
#include "fcgi.h"
#include "record.h"
#include "request.h"
#include "stream.h"

#define q1Debug() qDebug() << "[" << this->id << "]"
#define q1Critical() qCritical() << "[" << this->id << "]"
#define q2Debug(record) qDebug() << "[" << this->id << "," << record.getRequestId() << "]"
#define q2Critical(record) qCritical() << "[" << this->id << "," << record.getRequestId() << "]"

/*
 * Values for role component of FCGI_BeginRequestBody
 */
#define FCGI_RESPONDER  1
#define FCGI_AUTHORIZER 2
#define FCGI_FILTER     3

/*
 * Mask for flags component of FCGI_BeginRequestBody
 */
#define FCGI_KEEP_CONN  1

static int nextConnectionId = 0;

QFCgiConnection::QFCgiConnection(QIODevice *device, QFCgi *parent) : QObject(parent) {
  this->id = ++nextConnectionId;
  this->device = device;

  connect(this->device, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  connect(this->device, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

QFCgiConnection::~QFCgiConnection() {
  delete device;
}

int QFCgiConnection::getId() const {
  return this->id;
}

void QFCgiConnection::send(const QFCgiRecord &record) {
  q2Debug(record) << "sending record [ type:" << record.getType() << ", content-length:" << record.getContent().size() << "]";
  record.write(this->device);
}

void QFCgiConnection::closeConnection() {
  this->device->close();
}

void QFCgiConnection::onReadyRead() {
  fillBuffer();

  QFCgiRecord record;
  qint32 nconsumed;

  while ((nconsumed = record.read(this->buf)) > 0) {
    this->buf.remove(0, nconsumed);

    switch (record.getRequestId()) {
      case 0:  handleManagementRecord(record); break;
      default: handleApplicationRecord(record); break;
    }
  }

  if (nconsumed < 0) {
    q1Critical() << "failed to read record";
    deleteLater();
  }
}

void QFCgiConnection::onDisconnected() {
  q1Debug() << "FastCGI connection closed";
  deleteLater();
}

void QFCgiConnection::fillBuffer() {
  qint64 avail = this->device->bytesAvailable();
  char buf[avail];

  qint64 nread = this->device->read(buf, avail);

  if (nread >= 0) {
    q1Debug() << nread << "bytes read from socket";
    this->buf.append(buf, nread);
  } else {
    q1Critical() << this->device->errorString();
    deleteLater();
  }
}

void QFCgiConnection::handleManagementRecord(QFCgiRecord &record) {
  q1Debug() << "management record read" << record.getType() << record.getRequestId();
}

void QFCgiConnection::handleApplicationRecord(QFCgiRecord &record) {
  QFCgiRequest *request = this->requests.value(record.getRequestId(), 0);

  if (request == 0 && record.getType() != QFCgiRecord::FCGI_BEGIN_REQUEST) {
    q2Critical(record) << "no such request";
    deleteLater();
    return;
  }

  switch (record.getType()) {
    case QFCgiRecord::FCGI_BEGIN_REQUEST: handleFCGI_BEGIN_REQUEST(record); break;
    case QFCgiRecord::FCGI_PARAMS: handleFCGI_PARAMS(request, record); break;
    case QFCgiRecord::FCGI_STDIN: handleFCGI_STDIN(request, record); break;
    default: q2Critical(record) << "invalid record of type" << record.getType();
  }
}

void QFCgiConnection::handleFCGI_BEGIN_REQUEST(QFCgiRecord &record) {
  const QByteArray &ba = record.getContent();
  quint16 role = ((ba[0] & 0xFF) << 8) | (ba[1] & 0xFF);
  quint8 flags = ba[2];
  bool keep_conn = ((flags & FCGI_KEEP_CONN) > 0);

  if (role != FCGI_RESPONDER) {
    q2Critical(record) << "new FastCGI request (unsupported role) [ role:" << role << ", keep_conn:" << keep_conn << "]";
    QFCgiRecord response = QFCgiRecord::createEndRequest(record.getRequestId(), 0, QFCgiRecord::FCGI_UNKNOWN_ROLE);
    send(response);

    return;
  }

  QFCgiRequest *request = new QFCgiRequest(record.getRequestId(), keep_conn, this);
  this->requests.insert(request->getId(), request);
  q2Debug(record) << "new FastCGI request [ role:" << role << ", keep_conn:" << keep_conn << "]";
}

void QFCgiConnection::handleFCGI_PARAMS(QFCgiRequest *request, QFCgiRecord &record) {
  const QByteArray &ba = record.getContent();

  if (!ba.isEmpty()) {
    q2Debug(record) << "FCGI_PARAMS";
    request->consumeParamsBuffer(ba);
  } else {
    q2Debug(record) << "FCGI_PARAMS (end of stream)";
    QFCgi *fcgi = qobject_cast<QFCgi*>(parent());
    emit fcgi->newRequest(request);
  }
}

void QFCgiConnection::handleFCGI_STDIN(QFCgiRequest *request, QFCgiRecord &record) {
  const QByteArray &ba = record.getContent();

  if (!ba.isEmpty()) {
    q2Debug(record) << "FCGI_STDIN";
    request->in->append(ba);
  } else {
    q2Debug(record) << "FCGI_STDIN (end of stream)";
    request->in->setEof();
  }
}
