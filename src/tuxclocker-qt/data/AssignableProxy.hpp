#pragma once

// Acts as a buffer for unapplied assignables and applies them asynchronously

#include <AssignableConnection.hpp>
#include <Device.hpp>
#include <memory>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QObject>
#include <variant>

namespace TC = TuxClocker;

class AssignableProxy : public QObject {
public:
	AssignableProxy(QString path, QDBusConnection conn,
		QObject *parent = nullptr);
	void apply();
	void startConnection(std::shared_ptr<AssignableConnection> conn);
	// Stop connection and clear current connection
	void stopConnection();
	void setValue(QVariant v) {m_value = v;}
signals:
	void applied(std::optional<TC::Device::AssignmentError>);
	void connectionValueChanged(std::variant<QVariant, TC::Device::AssignmentError>,
		QString text);
	void connectionStarted();
	void connectionStopped();
private:
	Q_OBJECT
	
	QVariant m_value;
	QDBusInterface *m_iface;
	// This is a bit of a peril but not sure if we can store interfaces any better...
	std::shared_ptr<AssignableConnection> m_assignableConnection;

	std::optional<TC::Device::AssignmentError> doApply(const QVariant &v);
};