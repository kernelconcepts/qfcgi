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

#include <QTcpServer>
#include <QTcpSocket>

#include "connection.h"
#include "qfcgi.h"

QFCgi::QFCgi(QObject *parent) : QObject(parent) {
  this->server = new QTcpServer(this);
}

QFCgi::~QFCgi() {

}

void QFCgi::start() {
  this->server->listen(QHostAddress::Any, 9000);

  connect(this->server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));

  qDebug() << "FastCGI application started";
}

void QFCgi::onNewConnection() {
  QTcpSocket *so = this->server->nextPendingConnection();
  QFCgiConnection *connection = new QFCgiConnection(so, this);

  delete connection;
}
