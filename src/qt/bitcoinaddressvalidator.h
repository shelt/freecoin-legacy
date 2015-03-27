



#ifndef FREECOIN_QT_FREECOINADDRESSVALIDATOR_H
#define FREECOIN_QT_FREECOINADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class FreecoinAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit FreecoinAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Freecoin address widget validator, checks for a valid freecoin address.
 */
class FreecoinAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit FreecoinAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // FREECOIN_QT_FREECOINADDRESSVALIDATOR_H
