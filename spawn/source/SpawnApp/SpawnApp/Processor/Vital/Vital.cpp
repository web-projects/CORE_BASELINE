#include <iostream>

#include "Vital.h"
#include "..\mastercard\mccitmit.h"
#include "TSYSCofInfo.h"

Vital::Vital(TCProcessor* p) : Platform(p)
{

}

void Vital::runTransaction(StringHash& data, StringHash& store)
{
	std::cout << "vital: runTransaction\n";

	if (buildSaleReq(data, store))
	{
		std::cout << "vital: request has CMI value: " << store.get("CMI") << "\n";
	}
	else
	{
		std::cout << "vital: error->buildSaleReq()\n";
	}
}

bool Vital::buildSaleReq(StringHash& data, StringHash& store)
{
	bool retVal = false;

	char industryCode = Proc->merchant->getIndustryCode()[0];

	// CMI (CIT/MIT) Gv083 fits here
	if (data.check("mediatype", "mc") && industryCode == 'D')
	{
		bool send_mc_cit_mit_indicator = false;
		std::cout << "vital: buildSaleReq() - mediatype matches 'mc'\n";

		//ToDo: mocked
		StringHash transaction_snapshot;
		transaction_snapshot.put("action", "sale");
		transaction_snapshot.put("billingid", "y");
		transaction_snapshot.put("cit", "y");
		transaction_snapshot.put("cardonfileid", "y");

		bool acceptableStoreAction = data.check("action", "preauth") || data.check("action", "sale") || data.check("action", "emv_auth");
		if (acceptableStoreAction && data.check("store", "y"))
			send_mc_cit_mit_indicator = true;
		if (data.get("billingid"))
			send_mc_cit_mit_indicator = true;
		if (transaction_snapshot.check("action", "store") && transaction_snapshot.check("verify", "y"))
			send_mc_cit_mit_indicator = true;
		if (transaction_snapshot.check("action", "store") && transaction_snapshot.check("authnow", "y"))
			send_mc_cit_mit_indicator = true;
		if (data.check("cardonfile", "i"))
			send_mc_cit_mit_indicator = true;
		if (data.check("cardonfile", "r"))
			send_mc_cit_mit_indicator = true;
		if (data.check("cardonfile", "o"))
			send_mc_cit_mit_indicator = true;
		if (data.check("cardonfile", "t"))
			send_mc_cit_mit_indicator = true;
		if (data.check("cardonfile", "c"))
			send_mc_cit_mit_indicator = true;
		if (data.check("cardonfile", "p"))
			send_mc_cit_mit_indicator = true;

		if (send_mc_cit_mit_indicator)
		{
			char cmi[CMIDATA_LENGTH + 1] = { 0, 0, 0, 0, 0 };

			TSYSCofInfo tsysCofInfo(transaction_snapshot, Proc->transID, Proc->merchant->getIndustryCode()[0], "sale");
			tsysCofInfo.getCMIndicator(cmi, sizeof(cmi));
			if (cmi[0])
			{
				/*if (sent_group)
				{
					put(GS);
				}*/
				put("083", 3);
				//put(cmi);
				//put(FS);
				//sent_group = true;
				store.put("CMI", cmi);	// Set the indicator for the settlement
				//PANTHEIOS_LOG_WITH_TS(PANTHEIOS_SEV_DEBUG, dummy_tv, true, Proc->transID, ",", "CIT/MIT Indicator: ", cmi);

				retVal = true;
			}
		}
	}

	return retVal;
}