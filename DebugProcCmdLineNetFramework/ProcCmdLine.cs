using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;

public static class ProcCmdLine
{
	private static class Win32Native
	{
		//
		// Win32 Native Exports
		//
		[DllImport("shell32.dll", SetLastError = true, CharSet = CharSet.Unicode,
			EntryPoint = "CommandLineToArgvW")]
		public static extern IntPtr CommandLineToArgv(string lpCmdLine, out int pNumArgs);

		//
		// ProcCmdLine32.dll Exports
		//

		[DllImport("ProcCmdLine32.dll", CharSet = CharSet.Unicode, EntryPoint = "GetProcCmdLineW")]
		public extern static int GetProcCmdLine32W(uint nProcId, StringBuilder sb, uint dwSizeBuf);

		[DllImport("ProcCmdLine64.dll", CharSet = CharSet.Unicode, EntryPoint = "GetProcCmdLineW")]
		public extern static int GetProcCmdLine64W(uint nProcId, StringBuilder sb, uint dwSizeBuf);

		/* ANSI compatible exports, you shouldn't need these, but they are here if you do.
		[DllImport("ProcCmdLine32.dll", CharSet = CharSet.Ansi, EntryPoint = "GetProcCmdLineA",
			ThrowOnUnmappableChar = true, BestFitMapping = false)]
		public extern static int GetProcCmdLine32A(uint nProcId, StringBuilder sb, uint dwSizeBuf);

		[DllImport("ProcCmdLine64.dll", CharSet = CharSet.Ansi, EntryPoint = "GetProcCmdLineA",
			ThrowOnUnmappableChar = true, BestFitMapping = false)]
		public extern static int GetProcCmdLine64A(uint nProcId, StringBuilder sb, uint dwSizeBuf);
		*/
	}

	private static string ErrorToString(int error)
	{
		return new string[]
		{
			"Success",
			"Failed to open process",
			"Failed to resolve PEB address",
			"Failed to read process memory",
			"Failed to read UNICODE_STRING",
			"Failed to allocate memory",
			"Failed to read command line"
		}[Math.Abs(error)];
	}

	private static IReadOnlyList<string> CommandLineToArgs(string commandLine)
	{
		if (string.IsNullOrEmpty(commandLine)) { return Array.Empty<string>(); }

		var argv = Win32Native.CommandLineToArgv(commandLine, out var argc);
		if (argv == IntPtr.Zero)
		{
			throw new Win32Exception(Marshal.GetLastWin32Error());
		}
		try
		{
			var args = new string[argc];
			for (var i = 0; i < args.Length; ++i)
			{
				var p = Marshal.ReadIntPtr(argv, i * IntPtr.Size);
				args[i] = Marshal.PtrToStringUni(p);
			}
			return args.ToList().AsReadOnly();
		}
		finally
		{
			Marshal.FreeHGlobal(argv);
		}
	}

	public static string GetCommandLineOfProcessW(Process proc)
	{
		var sb = new StringBuilder(capacity: 0xFFFF);
		var rc = -1;
		switch (IntPtr.Size)
		{
			case 4:
				rc = Win32Native.GetProcCmdLine32W((uint)proc.Id, sb, (uint)sb.Capacity);
				break;
			case 8:
				rc = Win32Native.GetProcCmdLine64W((uint)proc.Id, sb, (uint)sb.Capacity);
				break;
		}
		return (0 == rc) ? sb.ToString() : throw new Win32Exception(rc, ErrorToString(rc));
	}

	public static IReadOnlyList<string> GetCommandLineArrayOfProcess(Process proc)
	{
		return CommandLineToArgs(GetCommandLineOfProcessW(proc));
	}

	private static string RebuildCommandLineFromArray(string[] arrArgs)
	{
		string encode(string s)
		{
			if (string.IsNullOrEmpty(s)) { return "\"\""; }
			return Regex.Replace(Regex.Replace(s, @"(\\*)" + "\"", @"$1\$0"), @"^(.*\s.*?)(\\*)$", "\"$1$2$2\"");
		}

		if ((arrArgs != null) && (arrArgs.Length > 0))
		{
			return string.Join(" ", Array.ConvertAll(arrArgs, encode));
		}

		return string.Empty;
	}
}
