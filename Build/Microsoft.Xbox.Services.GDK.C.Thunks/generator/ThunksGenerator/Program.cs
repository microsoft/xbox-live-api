using System;
using System.Collections.Generic;
using System.IO;
using System.Text.RegularExpressions;

namespace ThunksGenerator
{
    class Program
    {
        static void Main(string[] args)
        {
            var thunksDefFile = new FileInfo("../../../../dll/Microsoft.Xbox.Services.GDK.C.Thunks.def");
            Console.WriteLine(thunksDefFile.FullName);
            string xsapiReproFolder = thunksDefFile.Directory.Parent.Parent.Parent.FullName;

            string cHeadersFolder = Path.Combine(xsapiReproFolder, @"Include\xsapi-c\");
            var headerFiles = Directory.EnumerateFiles(cHeadersFolder, "*.h", SearchOption.AllDirectories);

            Console.WriteLine("Finding apis");
            List<string> fns = new List<string>();
            foreach (string curHeader in headerFiles)
            {
                // Remove non-GDK headers and headers that don't contain any APIs
                if (curHeader.Contains("pal.h") || curHeader.Contains("game_invite_c.h"))
                {
                    continue;
                }
                ProcessHeader(curHeader, fns);
            }

            fns.Sort();

            Console.WriteLine($"Writing apis to {thunksDefFile.FullName}");
            // Intentionally omit the "LIBRARY <name>.dll" statement. It was removed to fix
            // Bug 61446517 (the import lib referencing the wrong DLL name); the DLL name is
            // supplied by the linker instead. Re-adding it here would regress that fix.
            string content = "EXPORTS\n";
            foreach (string fn in fns)
            {
                string apiName = fn.Substring(0, fn.Length - 1);
                content += "    " + apiName + "\n";
            }
            content += "\n    XblWrapper_XblInitialize";
            content += "\n    XblWrapper_XblCleanupAsync";
            File.WriteAllText(thunksDefFile.FullName, content);
        }

        static void ProcessHeader(string curHeader, List<string> fns)
        {
            System.IO.StreamReader file = new System.IO.StreamReader(curHeader);
            while (true)
            {
                string line = file.ReadLine();
                if (line == null)
                    break;

                if (line.Contains("STDAPI"))
                {
                    line = line.Replace("STDAPI ", "");
                    line = line.Replace(") XBL_NOEXCEPT", "");
                    Regex regex = new Regex("STDAPI_(.+) ");
                    line = regex.Replace(line, "");
                    if (line.Contains(")"))
                    {
                        // Remove all the handlers
                        line = string.Empty;
                    }
                    if (line.Contains("XblLocalStorage") ||
                        line.Contains("XblAchievementUnlockAddNotificationHandler") ||
                        line.Contains("XblAchievementUnlockRemoveNotificationHandler") ||
                        line.Contains("XblEventsSetMaxFileSize") ||
                        line.Contains("XblEventsSetStorageAllotment") ||
                        line.Contains("XblMultiplayerActivityAddInviteHandler") ||
                        line.Contains("XblMultiplayerActivityRemoveInviteHandler") ||
                        line.Contains("XblNotificationSubscribeToNotificationsAsync") ||
                        line.Contains("XblNotificationUnsubscribeFromNotificationsAsync")
                        )
                    {
                        // Remove non-GDK APIs
                        continue;
                    }
                    int index = line.IndexOf("(");
                    if (index > 0)
                        line = line.Substring(0, index + 1);

                    if (!string.IsNullOrWhiteSpace(line))
                    {
                        line = line.Trim();
                        fns.Add(line);
                    }
                }
            }
        }
    }
}
