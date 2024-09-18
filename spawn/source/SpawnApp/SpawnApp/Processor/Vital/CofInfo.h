#pragma once

#include <string>

class StringHash;

class CofInfo
{
public:
    bool is_COF() const;

    bool isVisaCard() const;
    bool isMasterCard() const;
    bool isDiscoveryCard() const;
    bool isAmexCard() const;

    bool is_VISA_MC_DISC() const;

    bool isTypeRetail() const;
    bool isTypeEcomm() const;
    bool isTypeMoto() const;
    bool isTypeRestaurant() const;

    bool isFirstCofTransaction_y() const;
    bool isCIT_y() const;

    bool isAmountDue() const;
    bool isCardonfileidPresent() const;
    bool isCitadelMode() const;
    bool isFirst() const;
    bool isSubsequent() const;

    // COF Category 1:
    bool isFirstCIT() const;
    bool isSubseqCIT() const;
    bool isCIT() const;

    // COF Category 2:
    bool isFirstUCoF() const;
    bool isSubseqUCoF() const;
    bool isUCoF() const;

    // COF Category 3:
    bool isFirstRecurring() const;
    bool isSubseqRecurring() const;
    bool isRecurring() const;

    // COF Category 4:
    bool isFirstInstallment() const;
    bool isSubseqInstallment() const;
    bool isInstallment() const;

    // Note: d is a non-const reference
    //       m_data is a const reference to the same object

    CofInfo(StringHash& d,
        const std::string& transId,
        const char industryCode,
        const std::string& caller);

    bool isConsistent(const std::string& caller, int& line) const;
    std::string print() const;

    const std::string transId; // without m_ such that the dlog macros can access this

protected:
    const StringHash& m_data;
    const char m_industryCode;
    const std::string m_caller;

    ~CofInfo(); // we don't allow polymorphic usage, Core Guidelines C.35
};
