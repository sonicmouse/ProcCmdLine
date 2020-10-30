using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;

namespace DebugProcCmdLineNetFramework
{
	class Program
	{
		static void Main(string[] args)
		{
			Console.WriteLine("Test ProcCmdLine Library");
			Console.WriteLine("------------------------\n");

			foreach (var p in Process.GetProcesses())
			{
				Console.ForegroundColor = ConsoleColor.Green;
				Console.WriteLine($"{p.ProcessName} ({p.Id}):");
				Console.ResetColor();

				try
				{
					var workingDir = ProcCmdLine.GetWorkingDirectoryOfProcess(p);
                    Console.WriteLine("\tdir: " + workingDir);
					var cmdLineArray = ProcCmdLine.GetCommandLineArrayOfProcess(p);
					cmdLineArray.Select((x, i) => $"\targ {i}: {x}").ToList().ForEach(Console.WriteLine);
				}
				catch(Win32Exception ex)
				{
					Console.ForegroundColor = ConsoleColor.Red;
					Console.WriteLine($"\tError: {ex.Message}");
					Console.ResetColor();
				}
			}

			Console.Write("\nPress any key to quit ...");
			Console.ReadKey(true);
		}
	}
}
