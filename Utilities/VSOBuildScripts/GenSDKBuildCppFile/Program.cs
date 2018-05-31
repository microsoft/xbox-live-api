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
            if (args.Length != 3)
            {
                Console.WriteLine("Param1 = Folder to source root");
                Console.WriteLine("Param2 = File output");
                Console.WriteLine("Param3 = xbox | uwp");
            }

            string rootFolder = args[0];
            string outputFile = args[1];
            string buildType = args[2].ToLower();
            bool xboxBuild = buildType.Contains("xbox");

            string sourceFolder = Path.Combine(rootFolder, "Source");

            TextWriter tw = new StreamWriter(outputFile);

            tw.WriteLine("#include \"pch.h\"");
            tw.WriteLine("");

            string[] files = Directory.GetFiles(sourceFolder, "*.cpp", SearchOption.AllDirectories);
            foreach( string file in files )
            {
                bool skip = false;
                string line = file;

                // for all builds, skip certain files/folders
                if (
                    line.Contains(@"\WinRT") ||
                    (line.Contains(@"Source\System\") && !(line.Contains(@"Source\System\xbox_live_mutex.cpp") || line.Contains(@"Source\System\C\user_internal_c.cpp"))) ||
                    line.Contains(@"\Common\Server\")  || line.Contains(@"\a\") || line.Contains(@"\Unix\") || line.ToLower().Contains(@"\android\") ||
                    line.Contains(@"\u\") || line.Contains(@"\i\") ||
                    line.Contains(@"\iOS\")
                    )
                {
                    skip = true;
                }

                // for xbox build, skip certain files/folders
                if (xboxBuild &&
                        (
                        line.Contains(@"Common\Desktop\pch.cpp") ||
                        line.Contains(@"Common\Desktop\dllmain.cpp") ||
                        line.Contains(@"\Events\") ||
                        line.Contains(@"Misc\notification_service.cpp") ||
                        line.Contains(@"Misc\Windows\notification_service_windows.cpp") ||
                        line.Contains(@"Misc\UWP\title_callable_ui.cpp") ||
                        line.Contains(@"Presence\presence_writer.cpp")
                        )
                    )
                {
                    skip = true;
                }

                // for UWP build, skip certain files/folders
                if ( !xboxBuild &&
                        (
                        (line.Contains(@"\Common\Desktop\") && !line.Contains(@"Common\Desktop\XboxLiveContext_Desktop.cpp") && !line.Contains(@"Common\Desktop\pch.cpp")) ||
                        line.Contains(@"\Services\Marketplace") ||
                        line.Contains(@"Source\Shared\Desktop\local_config_desktop.cpp")
                        )
                    )
                {
                    skip = true;
                }

                if( !xboxBuild && 
                    (
                        line.Contains(@"Source\Shared\WinRT\local_config_winrt.cpp") ||
                        line.Contains(@"Source\Shared\WinRT\Event_WinRT.cpp") ||
                        line.Contains(@"Source\System\auth_config.cpp") ||
                        line.Contains(@"Source\System\token_and_signature_result.cpp") ||
                        line.Contains(@"Source\System\user.cpp") ||
                        line.Contains(@"Source\System\user_impl.cpp") ||
                        line.Contains(@"Source\System\user_impl_idp.cpp") ||
                        line.Contains(@"Source\System\xbox_live_mutex.cpp")
                    )
                  )
                {
                    skip = false;
                }

                if ( !skip )
                {
                    line = line.Replace(rootFolder, "#include \"..\\..");
                    line += "\"";
                    tw.WriteLine(line);
                }
            }

            tw.Close();
        }
    }
}
