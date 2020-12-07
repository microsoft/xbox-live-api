// Copyright (c) Microsoft Corporation
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace GenSDKBuildCppFile
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                Console.WriteLine("Param1 = Path to header");
                Console.WriteLine("Param2 = year | month | buildDate | qfe");
                return;
            }

            string fileName = args[0];
            string returnType = args[1];

            TextReader tr = new StreamReader(fileName);
            string headerFileText = tr.ReadToEnd();
            string defineToSearchFor = "XBOX_SERVICES_API_VERSION_STRING";
            int indexOfDefine = headerFileText.LastIndexOf(defineToSearchFor);
            if( indexOfDefine > 0 )
            {
                indexOfDefine += defineToSearchFor.Length;
                indexOfDefine += 2; // skip space and "

                string apiVersionText = headerFileText.Substring(indexOfDefine);
                int yearEndIndex = apiVersionText.IndexOf('.');
                string textAfterYear = apiVersionText.Substring(yearEndIndex + 1);
                int monthEndIndex = textAfterYear.IndexOf('.');
                string textAfterMonth = textAfterYear.Substring(monthEndIndex + 1);
                int buildDateEndIndex = textAfterMonth.IndexOf('.');
                string textAfterBuildDate = textAfterMonth.Substring(buildDateEndIndex + 1);
                int qfeEndIndex = textAfterBuildDate.IndexOf('"');

                string year = apiVersionText.Substring(0, yearEndIndex);
                string month = textAfterYear.Substring(0, monthEndIndex);
                string buildDate = textAfterMonth.Substring(0, buildDateEndIndex);
                string qfe = textAfterBuildDate.Substring(0, qfeEndIndex);

                if (returnType == "year")
                {
                    Console.WriteLine(year);
                }
                else if (returnType == "month")
                {
                    int monthNum = 0;
                    int.TryParse(month, out monthNum);
                    Console.WriteLine(monthNum.ToString("00"));
                }
                else if (returnType == "buildDate")
                {
                    Console.WriteLine(buildDate);
                }
                else if (returnType == "qfe")
                {
                    Console.WriteLine(qfe);
                }
            }
            else
            {
                Console.WriteLine("Can't find XBOX_SERVICES_API_VERSION_STRING");
            }
        }
    }
}
