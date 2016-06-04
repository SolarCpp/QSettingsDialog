#ifndef QTEXCEPTION_H
#define QTEXCEPTION_H

#include <exception>
#include <QString>

class QtException : public std::exception
{
public:
	QtException(const QString &what = QString(), int code = 0);

	QString qWhat() const;
	int code() const;
	const char *what() const final;

private:
	const QString error;
	const int eCode;
};

#endif // QTEXCEPTION_H
