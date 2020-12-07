using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;

namespace ExtractCodeSnip
{
    class Program
    {
        static void Help()
        {
            Console.WriteLine("ExtractCodeSnip.exe inputFolder outputFolder");
        }

        static void Main(string[] args)
        {
            if( args.Length < 2)
            {
                Help();
                return;
            }

            ReadFiles(args[0], args[1]);
        }

        static void ProcessFile(string outputFolder, string fileName)
        {
            List<string> codeSnipLines = new List<string>();
            codeSnipLines.Add("```cpp");
            string codeSnipName = string.Empty;
            var lines = File.ReadLines(fileName);
            bool inCodeSnippet = false;
            bool skipLines = false;
            int spaceOffset = 0;
            string l;

            Console.WriteLine("Reading {0}", fileName);
            foreach (string line in lines)
            {
                l = line;

                if (inCodeSnippet)
                {
                    // Remove leading spaces
                    if (!l.Contains("#"))
                    {
                        l = l.Substring(Math.Min(spaceOffset, l.Length));
                    }

                    if (l.Contains("CODE SNIPPET END"))
                    {
                        inCodeSnippet = false;
                        codeSnipLines.Add("```");
                        WriteCodeSnip(codeSnipLines, codeSnipName, outputFolder);
                        codeSnipName = string.Empty;
                        codeSnipLines.Clear();
                        codeSnipLines.Add("```cpp");
                    }
                    else
                    {
                        if (skipLines && l.Contains("CODE SKIP END"))
                        {
                            skipLines = false;
                        }
                        else if (!skipLines && l.Contains("CODE SKIP START"))
                        {
                            skipLines = true;
                        }
                        else if (l.Contains("DOTS"))
                        {
                            string dots = "";
                            for (int i = 0; i < l.Length; i++)
                            {
                                if (l[i] != ' ') break;
                                dots += " ";
                            }
                            dots += "...";

                            codeSnipLines.Add(dots);
                        }
                        else if (!l.Contains("CODE SNIP SKIP") && !skipLines)
                        {
                            string formatedLine = l.Replace("Data()->", "");
                            codeSnipLines.Add(formatedLine);
                        }
                    }
                }
                else if (l.Contains("CODE SNIPPET START"))
                {
                    spaceOffset = l.IndexOf('/');

                    string[] words = l.Split(':');
                    if (words.Length > 1)
                    {
                        inCodeSnippet = true;
                        codeSnipName = words[1].Trim();
                        Console.WriteLine("Snippet: '" + codeSnipName + "'");
                    }
                    else
                    {
                        Console.WriteLine("Error expected code snippet name:");
                        Console.WriteLine(l);
                    }

                }
            }

            WriteCodeSnip(codeSnipLines, codeSnipName, outputFolder);
        }

        static void ProcessFiles(string inputFolder, string outputFolder, string match)
        {
            var files = from file in Directory.EnumerateFiles(inputFolder, match, SearchOption.AllDirectories)
                        from line in File.ReadLines(file)
                        where line.Contains("CODE SNIPPET START")
                        select file;

            var noDupesFiles = files.Distinct().ToList();
            foreach (var f in noDupesFiles)
            {
                ProcessFile(outputFolder, f);
            }
        }

        static void ReadFiles(string inputFolder, string outputFolder)
        {
            Console.WriteLine("");
            Console.WriteLine("InputFolder: " + inputFolder);
            Console.WriteLine("OutputFolder: " + outputFolder);
            Console.WriteLine("");

            ProcessFiles(inputFolder, outputFolder, "*.cpp");
            ProcessFiles(inputFolder, outputFolder, "*.mm");
            ProcessFiles(inputFolder, outputFolder, "*.h");
        }

        static void WriteCodeSnip(List<string> codeSnipLines, string codeSnipName, string outputFolder)
        {
            if (codeSnipLines.Count > 0 && !String.IsNullOrWhiteSpace(codeSnipName))
            {
                codeSnipName += ".md";
                string path = Path.Combine(outputFolder, codeSnipName);
                System.IO.Directory.CreateDirectory(outputFolder);
                System.IO.File.WriteAllLines(path, codeSnipLines);
            }
        }
    }
}
