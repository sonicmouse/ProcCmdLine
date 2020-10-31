using System;
using System.Diagnostics;
using System.Linq;

namespace ManagedProcessCommandLine
{
	class Program
	{
		static void Main(string[] args)
		{
			Console.WriteLine("Managed ProcessCommandLine");
			Console.WriteLine("------------------------\n");

			foreach (var p in Process.GetProcesses())
			{
				Console.ForegroundColor = ConsoleColor.Green;
				Console.WriteLine($"{p.ProcessName} ({p.Id}):");
				Console.ResetColor();

				var rc = ProcessCommandLine.Retrieve(p, out var workingDir, ProcessCommandLine.Parameter.WorkingDirectory);
				if (0 == rc)
				{
					Console.WriteLine($"\tdir: {workingDir}");
				}
				else
				{
					Console.ForegroundColor = ConsoleColor.Red;
					Console.WriteLine($"\tUnable to get working directory ({rc}): {ProcessCommandLine.ErrorToString(rc)}");
					Console.ResetColor();
				}

				rc = ProcessCommandLine.Retrieve(p, out var cl);
				if (0 == rc)
				{
					var cmdLineArray = ProcessCommandLine.CommandLineToArgs(cl);
					cmdLineArray.Select((x, i) => $"\targ {i}:\t{x}").ToList().ForEach(Console.WriteLine);
				}
				else
				{
					Console.ForegroundColor = ConsoleColor.Red;
					Console.WriteLine($"\tUnable to get command line ({rc}): {ProcessCommandLine.ErrorToString(rc)}");
					Console.ResetColor();
				}
			}

			Console.Write("\nPress any key to quit ...");
			Console.ReadKey(true);
		}
	}
}
