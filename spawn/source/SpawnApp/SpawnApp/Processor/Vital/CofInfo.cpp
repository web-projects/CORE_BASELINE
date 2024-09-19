#include <fstream>
#include <sstream>

#include "CofInfo.h"
#include "..\..\Common\StringHash.h"

namespace
{
	const std::string version("version: 1.0 " __DATE__ " - " __TIME__);
	struct timespec dummy_tv;
}

CofInfo::~CofInfo() = default;

// Note: in case of citadel mode; citadel has to set this parameter! Don't add here!
bool CofInfo::is_COF() const
{
    static const bool isCOF = m_data.check("cardonfile", "y");
    return isCOF;
}

bool CofInfo::isVisaCard() const
{
    return is_COF() && m_data.check("mediatype", "visa");
}

bool CofInfo::isMasterCard() const
{
    return is_COF() && m_data.check("mediatype", "mc");
}

bool CofInfo::isDiscoveryCard() const
{
    return is_COF() && m_data.check("mediatype", "disc");
}

bool CofInfo::isAmexCard() const
{
    return is_COF() && m_data.check("mediatype", "amex");
}

bool CofInfo::is_VISA_MC_DISC() const
{
    return
        isVisaCard() ||
        isMasterCard() ||
        isDiscoveryCard();
}

bool CofInfo::isTypeEcomm() const
{
    return is_COF() &&
        (m_industryCode == 'D' && !m_data.check("type", "moto"));
}

bool CofInfo::isTypeRetail() const
{
    return is_COF() && m_industryCode == 'R';
}

bool CofInfo::isTypeMoto() const
{
    return is_COF() &&
        (m_industryCode == 'D' && m_data.check("type", "moto"));
}

bool CofInfo::isTypeRestaurant() const
{
    return is_COF() && m_industryCode == 'F';
}

bool CofInfo::isCitadelMode() const {
    return !m_data.getStr("billingid").empty() ||
        m_data.getStr("with_billingid_creation") == "y" ||
        m_data.getStr("store") == "y";
}

bool CofInfo::isFirstCofTransaction_y() const
{
    return m_data.check("firstcoftrans", "y");
}

bool CofInfo::isCIT_y() const
{
    return m_data.check("cit", "y");
}

bool CofInfo::isAmountDue() const
{
    return is_COF() && m_data.exists("amount") ? atoi(m_data.get("amount")) > 0 : false;
}

bool CofInfo::isCardonfileidPresent() const
{
    return is_COF() && m_data.exists("cardonfileid");
}

bool CofInfo::isFirstRecurring() const
{
    return
        is_COF() &&
        m_data.exists("paymentnumber") &&
        m_data.absent("totalpayments") &&
        isFirstCofTransaction_y();
}

bool CofInfo::isSubseqRecurring() const
{
    return
        is_COF() &&
        m_data.exists("paymentnumber") &&
        m_data.absent("totalpayments") &&
        !isFirstCofTransaction_y();
}

bool CofInfo::isRecurring() const
{
    return isFirstRecurring() || isSubseqRecurring();
}

bool CofInfo::isFirstInstallment() const
{
    return
        is_COF() &&
        isFirstCofTransaction_y() &&
        m_data.exists("paymentnumber") &&
        m_data.exists("totalpayments");
}

bool CofInfo::isSubseqInstallment() const
{
    return
        is_COF() &&
        m_data.exists("paymentnumber") &&
        m_data.exists("totalpayments") &&
        !isFirstCofTransaction_y();
}

bool CofInfo::isInstallment() const
{
    return isFirstInstallment() || isSubseqInstallment();
}

// UCOF = Unscheduled COF transaction
bool CofInfo::isFirstUCoF() const
{
    return
        is_COF() &&
        !isCIT_y() &&
        isFirstCofTransaction_y() &&
        m_data.absent("paymentnumber");
}

bool CofInfo::isSubseqUCoF() const
{
    return
        is_COF() &&
        !isCIT_y() &&
        !isFirstCofTransaction_y() &&
        m_data.absent("paymentnumber");
}

bool CofInfo::isUCoF() const
{
    return isFirstUCoF() || isSubseqUCoF();
}

// CIT = Cardholder Initiated Transaction
bool CofInfo::isFirstCIT() const
{
    return
        is_COF() &&
        isCIT_y() &&
        isFirstCofTransaction_y();
}

bool CofInfo::isSubseqCIT() const
{
    return
        is_COF() &&
        isCIT_y() &&
        !isFirstCofTransaction_y();
}

bool CofInfo::isCIT() const
{
    return isFirstCIT() || isSubseqCIT();
}

bool CofInfo::isFirst() const
{
    return
        isFirstRecurring() ||
        isFirstInstallment() ||
        isFirstUCoF() ||
        isFirstCIT();
}

bool CofInfo::isSubsequent() const
{
    return
        isSubseqRecurring() ||
        isSubseqInstallment() ||
        isSubseqCIT() ||
        isSubseqUCoF();
}

// Note: d is a non-const reference
//       m_data is a const reference to the same object

CofInfo::CofInfo(StringHash& d,
    const std::string& t,
    const char industryCode,
    const std::string& caller)
    : transId(t), m_data(d), m_industryCode(industryCode), m_caller(caller)
{
    int lineno;
    if (!isConsistent(caller, lineno))
    {

        std::ostringstream oss;
        oss << ": inconsistent COF values, context: " << caller
            << ", trigger-line: " << lineno
            << ", trans-id: " << d.getStr("transid") << ", "
            << ", bill-id: " << d.getStr("billingid") << ", "
            << ", platform: " << d.getStr("platform");

        // The following code is patch-up code to fix production code.
        // Once the root cause is determined and fixed, this code segment should be
        // deleted. Per spec, cardonfile is only allowed to be either 'y' or 'n'.
        // During production cardonfile=s was observed in batch file ?! This value
        // probably leaked in from legacy platforms/code that still use 's' and
        // other values and don't use supportedCOFvalues()
        if (isCitadelMode())
        {
            const std::string yn = m_data.getStr("cardonfile");
            if (!(yn == "y" || yn == "n" || yn == ""))
            {
                oss << ", cardonfile: <" << yn << ">";
                d.remove("cardonfile");
                d.put("cardonfile", "y");

                const std::string msg = "STRANGE CASE: INCONSISTENT COF VALUES: "
                    + oss.str();

                //PANTHEIOS_LOG_WITH_TS(PANTHEIOS_SEV_WARNING, dummy_tv, true,
                //    msg.c_str());
            }
        }
        else
        {
            const std::string msg = oss.str();
            throw std::logic_error(msg);
        }
    }
}

bool CofInfo::isConsistent(const std::string& caller, int& line) const
{
    line = 0;

    // if the parameter exists, it must be y or n
    auto yesno = [&](const char* const p, const int codeline) -> bool
    {
        line = codeline;

        if (m_data.exists(p))
        {
            const bool is_yn = m_data.check(p, "n") ||
                m_data.check(p, "y");

            if (!is_yn)
                line = codeline;

            return is_yn;
        }

        return true;
    };

    if (!yesno("cardonfile", __LINE__) ||
        !yesno("firstcoftrans", __LINE__) ||
        !yesno("cit", __LINE__) ||
        !yesno("with_billingid_creation", __LINE__) ||
        !yesno("store", __LINE__))
    {
        return false;
    }

    auto present = [&](const char* const p, const int codeline) -> bool
    {
        if (m_data.exists(p))
        {
            line = codeline;
            return true;
        }

        return false;
    };

    // if it's not a COF case, we don't want to see any COF related parameters:
    if ((m_data.absent("cardonfile") && caller != "settle")
        || m_data.check("cardonfile", "n"))
    {
        const bool isCOFrelated =
            present("billingid", __LINE__) ||
            present("cardonfileid", __LINE__) ||
            present("cit", __LINE__) ||
            present("with_billingid_creation", __LINE__) ||
            present("firstcoftrans", __LINE__);

        return !isCOFrelated;
    }

    return true;
};

std::string CofInfo::print() const
{
    const char* billingid = m_data.get("billingid");
    const char* cofid = m_data.get("cardonfileid");

    const std::string billid = billingid == NULL ? "not present" : billingid;
    const std::string cardonfileid = cofid == NULL ? "not present" :
        std::string("<") + std::string(cofid) + std::string(">");

    std::ostringstream oss;
    oss << version << "\n"
        << std::boolalpha << "is_COF = " << is_COF() << "\n"
        << "billingId: " << billid << "\n"
        << "cardonfileid: " << cardonfileid << "\n"
        << "isFirstCofTransaction_y = " << isFirstCofTransaction_y() << "\n"
        << "isCIT_y = " << isCIT_y() << "\n"
        << "withBillCreate_y = " << m_data.getStr("with_billingid_creation") << "\n"
        << "citadelMode = " << isCitadelMode() << "\n"
        << "isAmountDue() = " << isAmountDue() << "\n"
        << "isCardonfileidPresent() = " << isCardonfileidPresent() << "\n"
        << "---\nisVisaCard = " << isVisaCard() << "\n"
        << "isMasterCard = " << isMasterCard() << "\n"
        << "isDiscoveryCard = " << isDiscoveryCard() << "\n"
        << "isAmexCard = " << isAmexCard() << "\n"
        << "is_VISA_MC_DISC = " << is_VISA_MC_DISC() << "\n"
        << "mediatype = " << m_data.getStr("mediatype") << "\n"
        << "---\nisMoto = " << isTypeMoto() << "\n"
        << "isEComm = " << isTypeEcomm() << "\n"
        << "isRetail = " << isTypeRetail() << "\n"
        << "isRestaurant = " << isTypeRestaurant() << "\n"
        << "---\nisFirstRecurring = " << isFirstRecurring() << "\n"
        << "isSubseqRecurring = " << isSubseqRecurring() << "\n"
        << "isRecurring = " << isRecurring() << "\n"
        << "isFirstInstallment = " << isFirstInstallment() << "\n"
        << "isSubseqInstallment = " << isSubseqInstallment() << "\n"
        << "isInstallment = " << isInstallment() << "\n"
        << "isFirstUCoF = " << isFirstUCoF() << "\n"
        << "isSubseqUCoF = " << isSubseqUCoF() << "\n"
        << "isUCoF = " << isUCoF() << "\n"
        << "isFirstCIT = " << isFirstCIT() << "\n"
        << "isSubseqCIT = " << isSubseqCIT() << "\n"
        << "isCIT = " << isCIT() << "\n"
        << "isFirst = " << isFirst() << "\n"
        << "isSubsequent - " << isSubsequent() << "\n"
        ;

    return oss.str();
};
