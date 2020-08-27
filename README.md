# ProcCmdLine
A native Windows x86/x64 library to retrieve a command line from a process ID

In C#, you can import the library as so:

	[DllImport("ProcCmdLine32.dll", CharSet = CharSet.Unicode, EntryPoint = "GetProcCmdLineW")]
	public extern static int GetProcCmdLine32W(uint nProcId, StringBuilder sb, uint dwSizeBuf);

	[DllImport("ProcCmdLine64.dll", CharSet = CharSet.Unicode, EntryPoint = "GetProcCmdLineW")]
	public extern static int GetProcCmdLine64W(uint nProcId, StringBuilder sb, uint dwSizeBuf);

This is how you'd call it:

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
	