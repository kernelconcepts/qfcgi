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

#include "record.h"

/*
 * Number of bytes in a FCGI_Header. Future versions of the protocol
 * will not reduce this number.
 */
#define FCGI_HEADER_LEN 8

/*
 * Value for version component of FCGI_Header
 */
#define FCGI_VERSION_1 1

QFCgiRecord::QFCgiRecord() {
  QFCgiRecord(0, 0);
}

QFCgiRecord::QFCgiRecord(quint8 type, quint16 requestId) {
  this->version = QFCgiRecord::V1;
  this->type = type;
  this->requestId = requestId;
}

QFCgiRecord::QFCgiRecord(const QFCgiRecord &other) {
  this->version = other.version;
  this->type = other.type;
  this->requestId = other.requestId;
  this->content = other.content;
}

QFCgiRecord& QFCgiRecord::operator = (const QFCgiRecord &other) {
  this->version = other.version;
  this->type = other.type;
  this->requestId = other.requestId;
  this->content = other.content;

  return *this;
}

enum QFCgiRecord::Version QFCgiRecord::getVersion() const {
  return this->version;
}

bool QFCgiRecord::setVersion(quint8 version) {
  if (version == FCGI_VERSION_1) {
    this->version = QFCgiRecord::V1;
    return true;
  } else {
    return false;
  }
}

quint8 QFCgiRecord::getType() const {
  return this->type;
}

void QFCgiRecord::setType(quint8 type) {
  this->type = type;
}

quint16 QFCgiRecord::getRequestId() const {
  return this->requestId;
}

void QFCgiRecord::setRequestId(quint16 requestId) {
  this->requestId = requestId;
}

QByteArray& QFCgiRecord::getContent() {
  return this->content;
}

qint32 QFCgiRecord::read(const QByteArray &ba) {
  quint16 contentLength;
  quint8 paddingLength;

  qint32 nread = readHeader(ba, &contentLength, &paddingLength);

  if (nread <= 0) {
    return nread;
  }

  if (ba.size() < nread + contentLength + paddingLength) {
    return 0;
  }

  this->content = ba.mid(nread, contentLength);

  // don't read padding but skip it
  return nread + contentLength + paddingLength;
}

qint32 QFCgiRecord::readHeader(const QByteArray &ba, quint16 *contentLength, quint8 *paddingLength) {
  if (ba.size() < FCGI_HEADER_LEN) {
    // Not enough data available
    return 0;
  }

  if (!setVersion(ba[0] & 0xFF)) {
    return -1;
  }

  this->type = ba[1] & 0xFF;
  this->requestId = (ba[2] & 0xff << 8) | (ba[3] & 0xFF);

  *contentLength = (ba[4] & 0xff << 8) | (ba[5] & 0xFF);
  *paddingLength = ba[6] & 0xFF;

  // ba[7] -> reserved-flag

  return FCGI_HEADER_LEN;
}