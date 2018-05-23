using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace ProjectFileProcessor
{
    class Program
    {
        private static void ReadFile(string inputVcxproj, ref List<string> linesInputVcxproj)
        {
            try
            {
                using (StreamReader sr = new StreamReader(inputVcxproj))
                {
                    while (true)
                    {
                        string line = sr.ReadLine();
                        if (line == null)
                            break;
                        linesInputVcxproj.Add(line);
                    }
                }
            }
            catch (Exception e)
            {
                Console.WriteLine("The file could not be read:");
                Console.WriteLine(e.Message);
                return;
            }
        }

        private static void ExtractFileSection(
            ref List<string> linesInputVcxproj,
            ref List<string> filesInputVcxproj,
            string rootFolder,
            bool extraFilters
            )
        {
            bool fileSectionStart = false;
            bool fileSectionStop = false;
            rootFolder += "\\";
            for (int i = 1; i < linesInputVcxproj.Count - 1; i++)
            {
                string l0 = linesInputVcxproj[i - 1];
                string l1 = linesInputVcxproj[i];
                string l2 = linesInputVcxproj[i + 1];

                if (l1.Contains("<ItemGroup>") &&
                    (l2.Contains("ClInclude") || l2.Contains("ClCompile")))
                {
                    fileSectionStart = true;
                }

                if (fileSectionStart)
                {
                    if (l0.Contains("</ItemGroup>") && !l1.Contains("<ItemGroup>"))
                    {
                        fileSectionStop = true;
                    }
                }

                if (fileSectionStart && !fileSectionStop)
                {
                    string file = MakeFilePathRelative(l1, rootFolder);
                    if (extraFilters)
                    {
                        if (!file.Contains("ItemGroup") && !file.Contains("PrecompiledHeaderFile"))
                        {
                            filesInputVcxproj.Add(file);
                        }
                    }
                    else
                    {
                        filesInputVcxproj.Add(file);
                    }
                    //Console.WriteLine(l1);
                }
            }
        }

        class FileNode
        {
            public string cmake_vcxproj;
            public string template;
            public string output;
        }

        static void Main(string[] args)
        {
            string rootFolder = null;
            if (args.Length == 0)
                rootFolder = @"C:\git\forks\xbox-live-api";
            else
                rootFolder = args[0];
            
            if (args.Length == 4 && args[1] == "diff")
            {
                string fileOld = args[2];
                string fileNew = args[3];
                DiffFiles(fileOld, fileNew, rootFolder);
                return;
            }

            var fileNodes = new List<FileNode>();

            //Microsoft.Xbox.Services.141.UWP.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.UWP.Cpp.vcxproj",
                template = @"template-Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj",
            });

            //Microsoft.Xbox.Services.140.UWP.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.UWP.Cpp.vcxproj",
                template = @"template-Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj",
            });

            //Microsoft.Xbox.Services.141.XDK.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.XDK.Cpp.vcxproj",
                template = @"template-Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj",
            });

            //Microsoft.Xbox.Services.140.XDK.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.XDK.Cpp.vcxproj",
                template = @"template-Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj",
            });

            //Microsoft.Xbox.Services.141.XDK.WinRT
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.XDK.WinRT.vcxproj",
                template = @"template-Microsoft.Xbox.Services.141.XDK.WinRT.vcxproj",
                output = @"Microsoft.Xbox.Services.141.XDK.WinRT.vcxproj",
            });

            //Microsoft.Xbox.Services.140.XDK.WinRT
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.XDK.WinRT.vcxproj",
                template = @"template-Microsoft.Xbox.Services.140.XDK.WinRT.vcxproj",
                output = @"Microsoft.Xbox.Services.140.XDK.WinRT.vcxproj",
            });

            //Microsoft.Xbox.Services.140.UWP.WinRT
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.UWP.WinRT.vcxproj",
                template = @"template-Microsoft.Xbox.Services.140.UWP.WinRT.vcxproj",
                output = @"Microsoft.Xbox.Services.140.UWP.WinRT.vcxproj",
            });

            //Microsoft.Xbox.Services.110.XDK.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.XDK.Cpp.vcxproj",
                template = @"template-Microsoft.Xbox.Services.110.XDK.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.110.XDK.Cpp.vcxproj",
            });

            //Microsoft.Xbox.Services.UnitTest.140.TAEF
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.UnitTest.140.TAEF.vcxproj",
                template = @"template-Microsoft.Xbox.Services.UnitTest.140.TAEF.vcxproj",
                output = @"Microsoft.Xbox.Services.UnitTest.140.TAEF.vcxproj",
            });

            //Microsoft.Xbox.Services.UnitTest.140.TE
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.UnitTest.140.TE.vcxproj",
                template = @"template-Microsoft.Xbox.Services.UnitTest.140.TE.vcxproj",
                output = @"Microsoft.Xbox.Services.UnitTest.140.TE.vcxproj",
            });

            //Microsoft.Xbox.Services.110.XDK.Ship.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.Ship.vcxproj",
                template = @"template-Microsoft.Xbox.Services.110.XDK.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.110.XDK.Ship.Cpp.vcxproj",
            });

            //Microsoft.Xbox.Services.140.XDK.Ship.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.Ship.vcxproj",
                template = @"template-Microsoft.Xbox.Services.140.XDK.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.140.XDK.Ship.Cpp.vcxproj",
            });

            //Microsoft.Xbox.Services.141.XDK.Ship.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.Ship.vcxproj",
                template = @"template-Microsoft.Xbox.Services.141.XDK.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.141.XDK.Ship.Cpp.vcxproj",
            });

            //Microsoft.Xbox.Services.140.UWP.Ship.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.Ship.vcxproj",
                template = @"template-Microsoft.Xbox.Services.140.UWP.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.140.UWP.Ship.Cpp.vcxproj",
            });

            //Microsoft.Xbox.Services.141.UWP.Ship.Cpp
            fileNodes.Add(new FileNode
            {
                cmake_vcxproj = @"Microsoft.Xbox.Services.Ship.vcxproj",
                template = @"template-Microsoft.Xbox.Services.141.UWP.Cpp.vcxproj",
                output = @"Microsoft.Xbox.Services.141.UWP.Ship.Cpp.vcxproj",
            });

            foreach (FileNode fn in fileNodes)
            {
                var template_lines = new List<string>();
                var output_lines = new List<string>();
                var cmake_vcxproj_lines = new List<string>();
                var cmake_vcxproj_filters_lines = new List<string>();
                var cmake_vcxproj_filters_lines_filtered = new List<string>();
                var cmake_vcxproj_files = new List<string>();

                string cmake_vcxproj = Path.Combine(rootFolder, Path.Combine(@"Utilities\CMake\vcxprojs", fn.cmake_vcxproj));
                string cmake_vcxproj_filters_name = fn.cmake_vcxproj + ".filters";
                string cmake_vcxproj_filters = Path.Combine(rootFolder, Path.Combine(@"Utilities\CMake\vcxprojs", cmake_vcxproj_filters_name));
                string template = Path.Combine(rootFolder, Path.Combine(@"Utilities\CMake", fn.template));
                string output = Path.Combine(rootFolder, Path.Combine(@"Utilities\CMake\output", fn.output));
                string outputFilterName = fn.output + ".filters";
                string output_filters = Path.Combine(rootFolder, Path.Combine(@"Utilities\CMake\output", outputFilterName));

                FileInfo fiInput = new FileInfo(cmake_vcxproj);
                FileInfo fiInputFilters = new FileInfo(cmake_vcxproj_filters);
                Console.WriteLine("inputVcxproj: " + cmake_vcxproj);
                Console.WriteLine("template: " + template);
                Console.WriteLine("output: " + output);

                if (fiInput.Exists && fiInputFilters.Exists)
                {
                    ReadFile(cmake_vcxproj, ref cmake_vcxproj_lines);
                    ReadFile(cmake_vcxproj_filters, ref cmake_vcxproj_filters_lines);
                    ExtractFileSection(ref cmake_vcxproj_lines, ref cmake_vcxproj_files, rootFolder, false);
                    ReadFile(template, ref template_lines);
                    ReplaceFileSection(template_lines, cmake_vcxproj_files, ref output_lines);
                    if(fn.cmake_vcxproj.Contains(".Ship."))
                    {
                        ReplaceProjectName(ref output_lines, fn.output);
                    }
                    Console.WriteLine("Writing " + output);
                    WriteFile(output_lines, output);
                    cmake_vcxproj_filters_lines_filtered = ProcessFiltersFile(cmake_vcxproj_filters_lines, cmake_vcxproj_filters_lines_filtered, rootFolder, fn.output);
                    Console.WriteLine("Writing " + output_filters);
                    WriteFile(cmake_vcxproj_filters_lines_filtered, output_filters);
                }
                else
                {
                    Console.WriteLine("Skipping");
                }

                Console.WriteLine("");
            }
        }

        private static void ReplaceProjectName(ref List<string> output_lines, string outputFileName)
        {
            for (int i = 0; i < output_lines.Count; i++)
            {
                if (output_lines[i].Contains("<ProjectName>"))
                {
                    string projectName = outputFileName.Replace(".vcxproj", "");
                    output_lines[i] = "    <ProjectName>" + projectName + "</ProjectName>";
                }

                if (output_lines[i].Contains("<ProjectGuid>"))
                {
                    string projectGuid;
                    if (outputFileName.Contains("XDK"))
                    {
                        projectGuid = "{20E87245-DA60-40E5-9938-ABB445E78467}";
                    }
                    else
                    {
                        projectGuid = "{47FF466B-C455-48C0-8D89-37E3FC0897F8}";
                    }
                    output_lines[i] = "    <ProjectGuid>" + projectGuid + "</ProjectGuid>";
                }
            }
        }

        public static string ReplaceString(string str, string oldValue, string newValue, StringComparison comparison)
        {
            StringBuilder sb = new StringBuilder();

            int previousIndex = 0;
            int index = str.IndexOf(oldValue, comparison);
            while (index != -1)
            {
                sb.Append(str.Substring(previousIndex, index - previousIndex));
                sb.Append(newValue);
                index += oldValue.Length;

                previousIndex = index;
                index = str.IndexOf(oldValue, index, comparison);
            }
            sb.Append(str.Substring(previousIndex));

            return sb.ToString();
        }

        private static string MakeFilePathRelative(string inputFile, string rootFolder)
        {
            //     <ClInclude Include="C:\git\forks\xbox-live-api\Source\Services\Common\Desktop\pch.h" />
            // to
            //     <ClInclude Include="$(MSBuildThisFileDirectory)..\..\Source\Services\Common\Desktop\pch.h" />

            string filteredFile = ReplaceString(inputFile, rootFolder, @"$(MSBuildThisFileDirectory)..\..\", StringComparison.OrdinalIgnoreCase);
            filteredFile = filteredFile.Replace(" Condition=\"'$(Configuration)|$(Platform)'=='Debug|Win32'\"", "");
            filteredFile = filteredFile.Replace(@"..\..\Utilities\CMake\build\", "");
            filteredFile = filteredFile.Replace("\"  />", "\" />");

            return filteredFile;
        }

        private static void ReplaceFileSection(
            List<string> template_lines,
            List<string> cmake_vcxproj_files,
            ref List<string> output_lines)
        {
            for (int i = 0; i < template_lines.Count; i++)
            {
                string l = template_lines[i];
                if (l.Contains("****INSERTFILES****"))
                {
                    foreach (string s in cmake_vcxproj_files)
                    {
                        output_lines.Add(s);
                    }
                }
                else
                {
                    output_lines.Add(l);
                }
            }
        }

        private static void WriteFile(List<string> output_lines, string outputFile)
        {
            FileInfo fi = new FileInfo(outputFile);
            Directory.CreateDirectory(fi.DirectoryName);
            using (StreamWriter outputWriter = new StreamWriter(outputFile))
            {
                foreach (string line in output_lines)
                {
                    outputWriter.WriteLine(line);
                }
            }
        }

        private static List<string> ProcessFiltersFile(List<string> lines, List<string> lines_filtered, string rootFolder, string filePath)
        {
            rootFolder += "\\";
            foreach (string line in lines)
            {
                string lineOutput = MakeFilePathRelative(line, rootFolder);

                if(filePath.Contains(".110."))
                {
                    lineOutput = lineOutput.Replace("ToolsVersion=\"15.0\"", "ToolsVersion=\"4.0\"");
                }

                lineOutput = lineOutput.Replace("<Filter>Header Files</Filter>", "<Filter>C++ Public Includes</Filter>");
                lineOutput = lineOutput.Replace("<Filter Include=\"Header Files\">", "<Filter Include=\"C++ Public Includes\">");

                if (filePath.Contains(".XDK."))
                {
                    if (lineOutput.Contains("</Project>"))
                    {
                        lines_filtered.Add(@"  <ItemGroup>");
                        lines_filtered.Add("    <None Include=\"..\\..\\Source\\Shared\\Logger\\ERA_ETW.man\">");
                        lines_filtered.Add(@"      <Filter>C++ source\Shared\Logger</Filter>");
                        lines_filtered.Add(@"    </None>");
                        lines_filtered.Add(@"  </ItemGroup>");
                        lines_filtered.Add(@"  <ItemGroup>");
                        lines_filtered.Add("    <ResourceCompile Include=\"..\\..\\Source\\Shared\\Logger\\ERA_ETW.rc\">");
                        lines_filtered.Add(@"      <Filter>C++ source\Shared\Logger</Filter>");
                        lines_filtered.Add(@"    </ResourceCompile>");
                        lines_filtered.Add(@"  </ItemGroup>");
                    }
                }

                lines_filtered.Add(lineOutput);
            }


            // Need to sort filters file manually due to CMake bug: https://cmake.org/Bug/view.php?id=10481
            List<string> filterLines = new List<string>();
            List<List<string>> filterNodes = new List<List<string>>();
            bool captureFilters = false;
            for (int i = 0; i < lines_filtered.Count; i++)
            {
                string l1 = lines_filtered[i];

                if (l1.Contains("<ItemGroup>"))
                {
                    filterLines.Add(l1);
                    captureFilters = true;
                    i++;
                    l1 = lines_filtered[i];
                }

                if (l1.Contains("</ItemGroup>"))
                {
                    //Console.WriteLine("Start");
                    //foreach (var l in filterNodes)
                    //{
                    //    Console.WriteLine(l[0]);
                    //}
                    var sortedList = filterNodes.OrderBy(x => x[0]);
                    //Console.WriteLine("Final");
                    //foreach (var l in sortedList)
                    //{
                    //    Console.WriteLine(l[0]);
                    //}
                    foreach (var l in sortedList)
                    {
                        foreach (var s in l)
                        {
                            filterLines.Add(s);
                        }
                    }
                    filterNodes.Clear();
                    captureFilters = false;
                }

                if (captureFilters)
                {
                    List<string> filter = new List<string>();
                    filter.Add(lines_filtered[i]);
                    filter.Add(lines_filtered[i + 1]);
                    filter.Add(lines_filtered[i + 2]);
                    i += 2;

                    filterNodes.Add(filter);
                }
                else
                {
                    filterLines.Add(l1);
                }
            }

            return filterLines;
        }

        private static void DiffFiles(string fileOld, string fileNew, string rootFolder)
        {
            FileInfo fi1 = new FileInfo(fileOld);
            FileInfo fi2 = new FileInfo(fileNew);
            if (!fi1.Exists)
            {
                Console.WriteLine("Missing: " + fileOld);
                return;
            }
            if (!fi2.Exists)
            {
                Console.WriteLine("Missing: " + fileNew);
                return;
            }


            var fileOld_lines = new List<string>();
            var fileOld_files = new List<string>();
            var fileOld_files_pre = new List<string>();
            ReadFile(fileOld, ref fileOld_lines);
            ExtractFileSection(ref fileOld_lines, ref fileOld_files_pre, rootFolder, true);
            foreach (string sOld in fileOld_files_pre)
            {
                var s = sOld.Replace("$(MSBuildThisFileDirectory)", "");
                s = s.Replace("\"  />", "\" />");
                s = s.Replace("<", "");
                s = s.Replace("/>", "");
                s = s.Replace(">", "");
                s = s.Trim();
                s = s.ToLower();
                fileOld_files.Add(s);
            }
            var org_fileOld_files = new List<string>(fileOld_files);

            var fileNew_lines = new List<string>();
            var fileNew_files_pre = new List<string>();
            var fileNew_files = new List<string>();
            ReadFile(fileNew, ref fileNew_lines);
            //    <ClInclude Include="..\..\Include\xsapi\achievements.h" />
            //     <ClCompile Include="..\..\Source\Services\Achievements\achievement.cpp"  />
            ExtractFileSection(ref fileNew_lines, ref fileNew_files_pre, rootFolder, true);
            foreach (string sNew in fileNew_files_pre)
            {
                var s = sNew.Replace("$(MSBuildThisFileDirectory)", "");
                s = s.Replace("\"  />", "\" />");
                s = s.Replace("<", "");
                s = s.Replace("/>", "");
                s = s.Replace(">", "");
                s = s.Trim();
                s = s.ToLower();
                fileNew_files.Add(s);
            }
            var org_fileNew_files = new List<string>(fileNew_files);

            foreach (string sOld in fileOld_files)
            {
                if (sOld.Contains("achievements.h"))
                {
                    bool contains = fileNew_files.Contains(sOld);
                    fileNew_files.Remove(sOld);
                }

                fileNew_files.Remove(sOld);
            }
            foreach (string sNew in org_fileNew_files)
            {
                fileOld_files.Remove(sNew);
            }

            Console.WriteLine("Diffing:");
            Console.WriteLine("    " + fileOld);
            Console.WriteLine("    " + fileNew);
            foreach (string sOld in fileOld_files)
            {
                Console.WriteLine("Missing: " + sOld);
            }
            foreach (string sNew in fileNew_files)
            {
                Console.WriteLine("New: " + sNew);
            }
            Console.WriteLine("");
            Console.WriteLine("");
        }

    }
}

