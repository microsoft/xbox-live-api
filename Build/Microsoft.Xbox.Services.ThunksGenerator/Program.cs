using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.IO;
using System.Text.RegularExpressions;

namespace ThunksGenerator
{
    class Program
    {
        static Dictionary<string, IEnumerable<string>> PlatformSpecificFunctions = new Dictionary<string, IEnumerable<string>>();

        static void Main(string[] args)
        {
            PlatformSpecificFunctions.Add("HCHttpCallRequestSetSSLValidation", new List<string> { "win32", "gdk" });
            PlatformSpecificFunctions.Add("HCHttpDisableAssertsForSSLValidationInDevSandboxes", new List<string> { "xdk" });
            PlatformSpecificFunctions.Add("HCHttpCallRequestEnableGzipCompression", new List<string> { }); // Xal specifically disables gzip functionality
            PlatformSpecificFunctions.Add("XblNotificationSubscribeToNotificationsAsync", new List<string> { "android", "ios", "uwp" });
            PlatformSpecificFunctions.Add("XblNotificationUnsubscribeFromNotificationsAsync", new List<string> { "android", "ios", "uwp" });

            var thunksDefFile = new FileInfo("../../../Microsoft.Xbox.Services.Win32.C.Dll.def");
            Console.WriteLine(thunksDefFile.FullName);
            string xsapiReproFolder = thunksDefFile.Directory.Parent.FullName;

            string cHeadersFolder = Path.Combine(xsapiReproFolder, @"Include\xsapi-c\");
            var headerFiles = Directory.EnumerateFiles(cHeadersFolder, "*.h", SearchOption.AllDirectories).ToList();

            cHeadersFolder = Path.Combine(xsapiReproFolder, @"External\xal\Source\Xal\Include\Xal\");
            headerFiles.AddRange(Directory.EnumerateFiles(cHeadersFolder, "*.h", SearchOption.AllDirectories));

            cHeadersFolder = Path.Combine(xsapiReproFolder, @"External\xal\External\libHttpClient\Include\");
            headerFiles.AddRange(Directory.EnumerateFiles(cHeadersFolder, "*.h", SearchOption.AllDirectories));

            Regex regPlatform = new Regex(@".+(\\|/|_)(?<platform>win32|gdk|xdk|android|apple|grts|gsdk|uwp|jvm|internal)(_.+)?\.h", RegexOptions.IgnoreCase);

            Console.WriteLine("Finding apis");
            List<string> fns = new List<string>();
            foreach (string curHeader in headerFiles)
            {
                Match match = regPlatform.Match(curHeader);
                if (match.Success && !string.Equals(match.Result("${platform}"), "win32", StringComparison.CurrentCultureIgnoreCase))
                    continue;
                ProcessHeader(curHeader, fns);
            }

            var cleanfns = fns.Distinct().OrderBy(fn => fn);

            Console.WriteLine($"Writing apis to {thunksDefFile.FullName}");
            string content = "LIBRARY xsapi\n";
            content += "EXPORTS\n";
            foreach (string fn in cleanfns)
            {
                content += "    " + fn + "\n";
            }
            File.WriteAllText(thunksDefFile.FullName, content);
        }

        static void ProcessHeader(string curHeader, List<string> fns)
        {
            System.IO.StreamReader file = new System.IO.StreamReader(curHeader);
            string contents = file.ReadToEnd();

            Regex regSTDAPI = new Regex(@"STDAPI\s+(?<function>.+)\(");
            MatchCollection matches = regSTDAPI.Matches(contents);
            foreach( Match match in matches )
            {
                string fn = match.Result("${function}");
                if ( !PlatformSpecificFunctions.ContainsKey(fn) || PlatformSpecificFunctions[fn].Contains("win32") )
                    fns.Add(fn);
            }

            Regex regSTDAPI_ = new Regex(@"STDAPI_\((.+)\)\s+(?<function>.+)\(");
            matches = regSTDAPI_.Matches(contents);
            foreach (Match match in matches)
            {
                string fn = match.Result("${function}");
                if (!PlatformSpecificFunctions.ContainsKey(fn) || PlatformSpecificFunctions[fn].Contains("win32"))
                    fns.Add(fn);
            }
            /*
            while (true)
            {
                string line = file.ReadLine();
                if (line == null)
                    break;

                Match match = regSTDAPI.Match(line);
                if (!match.Success)
                    match = regSTDAPI.Match(line);
                if (match.Success)
                    fns.Add(match.Result("${function}"));
                else if( line.Contains("STDAPI") )
                {

                }
            }
            */
        }
    }
}
