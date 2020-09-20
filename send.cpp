#include <iostream>
#include <stdlib.h>
#include <string>
#include <cups/cups.h>
#include <fcgi_stdio.h>
#include <cgicc/Cgicc.h>

using namespace std;
using namespace cgicc;


void start_print(string copies, string color, file_iterator file)
{
	cups_dest_t *dest = cupsGetNamedDest(CUPS_HTTP_DEFAULT, NULL, NULL);
	cups_dinfo_t *info = cupsCopyDestInfo(CUPS_HTTP_DEFAULT, dest);

	int job_id = 0;
	int num_options = dest->num_options;
	cups_option_t *options = dest->options;
	num_options = cupsAddOption(CUPS_COPIES, copies.c_str(), num_options, &options);
	num_options = cupsAddOption("Ink", color == "1" ? "MONO" : "COLOR", num_options, &options);
	
	if (cupsCreateDestJob(CUPS_HTTP_DEFAULT, dest, info, &job_id, file->getName().c_str(), num_options, options) == IPP_STATUS_OK)
	{
		cout << "Create job: " << job_id << endl;
		if (cupsStartDestDocument(CUPS_HTTP_DEFAULT, dest, info, job_id, file->getName().c_str(), file->getDataType().c_str(), num_options, options, 1) == HTTP_STATUS_CONTINUE)
		{
			string data = file->getData();
			cupsWriteRequestData(CUPS_HTTP_DEFAULT, data.c_str(), file->getDataLength());
			if (cupsFinishDestDocument(CUPS_HTTP_DEFAULT, dest, info) == IPP_STATUS_OK)
				cout << "Finished printing" << endl;
			else
				cout << "Failed to print: " << cupsLastErrorString() << endl;
		}
	}
	else
	{
		cout << "Failed to create job: " << cupsLastErrorString() << endl;
	}
}


int main()
{
	while (FCGI_Accept() >= 0)
	{
		Cgicc data;
		file_iterator file = data.getFile("file");
		form_iterator f_color = data.getElement("color");
		form_iterator f_number = data.getElement("number");

		cout << "Content-type:text/html\r\n\r\n";
		start_print(**f_number, **f_color, file);
	}
	return 0;
}