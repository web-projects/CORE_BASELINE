#include <cstring>

#include "mccitmit.h"

#include "..\..\Vital\CofInfo.h"
#include "..\..\..\Common\StringHash.h"

mccitmit::mccitmit(StringHash& data,
    const std::string& transId,
    const char industryCode,
    const std::string& caller,
    std::string platform)
    : CofInfo(data, transId, industryCode, caller)
{
    _platform = platform;
}

char* mccitmit::getCMIndicator(char* buffer, unsigned int len) const
{
    if (buffer == nullptr || !len)
        return buffer;

    if (len < CMIDATA_LENGTH + 1)
    {
        buffer[0] = 0;
        return buffer;
    }

    if (m_data.check("action", "credit2"))
    {
        buffer[0] = 0;
        return buffer;
    }
    const char* tag(nullptr); // to be assigned ONCE later.
    const char* action = m_data.get("action");
    const char* billingId = m_data.get("billingid");
    const char* cardOnFile = m_data.get("cardonfile");
    const char* cardonfileId = m_data.get("cardonfileid");
    bool action_emvauth = (action != nullptr) && !strcmp(action, "emv_auth");
    bool action_preauth = (action != nullptr) && !strcmp(action, "preauth");
    bool action_sale = (action != nullptr) && !strcmp(action, "sale");
    bool action_store = (action != nullptr) && !strcmp(action, "store");
    bool action_verify = (action != nullptr) && !strcmp(action, "verify");
    bool authnow = m_data.check("authnow", "y");
    bool cof_c = false;
    bool cof_i = false;
    bool cof_o = false;
    bool cof_p = false;
    bool cof_r = false;
    bool cof_s = false;
    bool cof_t = false;
    bool cof_y = (cardOnFile != nullptr) && !strcmp(cardOnFile, "y");
    bool done = false;
    bool has_BillingId = false;
    bool has_CardOnFileId = false;
    bool isCIT = m_data.check("cit", "y");
    bool store_y = m_data.check("store", "y");
    unsigned int paymentNo = m_data.get_atoi("paymentnumber");
    unsigned int paymentQty = m_data.get_atoi("totalpayments");

    if (action != nullptr && !action_store)
    {
        action_sale = !strcmp(action, "sale");
        if (!action_sale)
        {
            action_verify = !strcmp(action, "verify");
            if (!action_verify)
            {
                action_preauth = !strcmp(action, "preauth");
                if (!action_preauth)
                {
                    action_emvauth = !strcmp(action, "emv_auth");
                }
            }
        }
    }

    if (billingId && billingId[0] == 'y')
    {
        has_BillingId = true;
    }

    if (cardonfileId && cardonfileId[0] == 'y')
    {
        has_CardOnFileId = true;
    }

    if (cardOnFile != nullptr && !cof_y)
    {
        cof_s = !strcmp(cardOnFile, "s");
        if (!cof_s)
        {
            cof_r = !strcmp(cardOnFile, "r");
            if (!cof_r)
            {
                cof_i = !strcmp(cardOnFile, "i");
                if (!cof_i)
                {
                    cof_o = !strcmp(cardOnFile, "o");
                    if (!cof_o)
                    {
                        cof_p = !strcmp(cardOnFile, "p");
                        if (!cof_p)
                        {
                            cof_c = !strcmp(cardOnFile, "c");
                            if (!cof_c)
                            {
                                cof_t = !strcmp(cardOnFile, "t");
                            }
                        }
                    }
                }
            }
        }
    }

    if (isCIT) // Is a cardholder initiated transaction (CIT) ?
    {
        if (action_store && m_data.get("cycle"))
        {
            if (paymentQty > 0)
            {
                // Explicit or Infered installment.
                if (cof_i || authnow || action_verify)
                {
                    tag = "C104"; // It is an installment.
                    done = true;
                }
            }
            else // "totalpayments" value is NULL or 0.
            {
                // Explicit or Infered recurring.
                if (cof_r || authnow || action_verify)
                {
                    tag = "C103"; // It is a subscription (recurring).
                    done = true;
                }
            }
        }

        if (!done && cof_r && (action_sale || action_preauth) && store_y) // Added as case 1 of Story 42609
        {
            tag = "C103"; // It is also a recurring transaction.
            done = true;
        }

        if (!done && cof_o)
        {
            if (((action_sale || action_preauth) && store_y) // Mid term added, NOT IN the original spec.doc
                || (action_store && m_data.check("verify", "y")) || action_verify) // This fixed MIT_CIT Bug 43549
            {
                tag = "C102"; // It is a standing order (i.e. debit).
                done = true;
            }
        }

        if (!done && (action_sale || action_preauth) && has_BillingId && paymentNo > 0) // All if() NOT IN the original spec.doc
        {
            if (paymentQty > 1)
                tag = "C104"; // It is an installment.
            else
                tag = "C103"; // It is a subscription.

            done = true;
        }
        if (!done && (action_sale || action_preauth || action_verify) && cof_o) // Bug 104983
        {
            tag = "C102";
            done = true;
        }

        if (!done && (cof_y || cof_s) &&
            (((action_preauth || action_sale || action_emvauth) && m_data.check("store", "y")) ||
                ((action_preauth || action_sale) && has_BillingId)))
        {
            tag = "C101"; //  It is the default CIT ad hoc.
            done = true;
        }
        if (!done && (cof_i) &&
            (((action_preauth || action_sale || action_emvauth) && m_data.check("store", "y")) ||
                ((action_preauth || action_sale) && has_BillingId)))
        {
            tag = "C104"; //  It is the default CIT ad hoc.
            done = true;
        }

        if (!done && (((action_preauth || action_sale || action_emvauth) && m_data.check("store", "y")) ||
            ((action_preauth || action_sale) && has_BillingId)))
        {
            tag = "C101"; //  It is the default CIT ad hoc.
            done = true;
        }
    }
    else //  merchant initiated transaction (MIT)
    {
        if ((action_preauth || action_sale) && has_BillingId && has_CardOnFileId &&
            paymentNo > 0 && paymentQty > 1)
        {
            tag = "M104"; // It is an specific case of installment (MIT_CIT story 42615).
            done = true;
        }

        if ((action_preauth || action_sale) &&
            (has_BillingId || (has_CardOnFileId && m_data.get("cc") && m_data.get("exp"))))
        {
            if (paymentNo > 0)
            {
                done = true;
                if (cof_i || paymentQty > 1)
                {
                    tag = "M104"; // Installment typical case.
                }
                else if (cof_r || paymentQty < 1)
                {
                    tag = "M103"; // It is a subscription (recurring).
                }
                else if (cof_c && (action_preauth || action_sale)) // MIT_CIT story 42619
                {
                    tag = "M207"; // Cancellation --> No show Charge
                }
                else if (cof_t && (action_preauth || action_sale)) // MIT_CIT story 42620
                {
                    tag = "M208"; // Transit debt recovery --> Resubmission
                }
                else if (cof_y || cof_s)
                {
                    tag = "M101"; // Auto top up  == Unscheduled COF
                }
                else
                    done = false;
            }
            else // no payment number found
            {
                done = true;
                if (cof_o)
                {
                    tag = "M102"; // It is a standing order (i.e. debit).
                }
                else if (cof_p)
                {
                    tag = "M205"; // It is a Partial Shipment.
                }
                else if (cof_c && (action_preauth || action_sale)) // MIT_CIT story 42619
                {
                    tag = "M207"; // Cancellation --> No show Charge
                }
                else if (cof_t && (action_preauth || action_sale)) // MIT_CIT story 42620
                {
                    tag = "M208"; // Transit debt recovery --> Resubmission
                }
                else if (cof_y || cof_s)
                {
                    tag = "M101"; // Auto top up  == Unscheduled COF
                }
                else
                    done = false;
            }
        }
        else if (!done && (action_preauth || action_sale))
        {
            // MIT_CIT story 42612
            if (cardOnFile == nullptr || cof_s)
            {
                tag = "M101";
                done = true;
            }
        }
    }

    // Fallback if the previous logic doesn't catch all the cases.
    if (!done)
    {
        if (isCIT)
        {
            if (cof_y || cof_s)
                tag = "C101";
            else if (cof_o)
                tag = "C102";
            else if (cof_r)
                tag = "C103";
            else if (cof_i)
                tag = "C104";
        }
        else if (m_data.check("cit", "n"))
        {
            if (cof_y || cof_s)
                tag = "M101";
            else if (cof_o)
                tag = "M102";
            else if (cof_r)
                tag = "M103";
            else if (cof_i)
                tag = "M104";
            else if (cof_p)
                tag = "M205";
            else if (cof_c)
                tag = "M207";
            else if (cof_t)
                tag = "M208"; // Transit debt recovery --> Resubmission
        }
    }

    if (!tag) // Safety. Should not happen.
    {
        buffer[0] = 0;
        return buffer;
    }

    strncpy_s(buffer, len, tag, len - 1);
    buffer[len - 1] = '\0';
    return buffer;
}
