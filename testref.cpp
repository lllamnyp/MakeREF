#include <stdio.h>
#include <string.h>

const int maxN = 2;			// Max No. of experiments

int f_idx[maxN][8];			// File index
int n = 2;

FILE * argsfile;

char buffer[256];

int string_trim(char *source)	// Trims a trailing backslash; path names must be terminated
{								// with spaces before closing double quote (implement in OPUS)
	char *end;
	size_t len = strlen(source);
	
	while ((len > 0) && ((source[len-1] == ' ') || (source[len-1] == '\\'))) len--;
	source[len] = '\0';

	return 0;
}

int main ( int argc, char *argv[] )
{
	argsfile = fopen("C:\\test\\log.txt", "w");
	fprintf(argsfile, "Arguments found:\n");
	for (int i = 0; i < argc; i++)
	{
		fprintf(argsfile, "\"%s\"\n", argv[i]);
	}
	fprintf(argsfile, "\nArguments after parsing:\n");
	
	sscanf(argv[1], "%d", &n);		// 1st arg: No. of exps (immediately stored in n)
									// 2nd arg: filename (stored in argv[2] without modification)
	string_trim(argv[3]);			// 3rd arg: 2-col tables dir, backslash stripped
	string_trim(argv[4]);			// 4th arg: measurement dir, backslash stripped
									// 5th arg: Sample name (argv[5])
									// 6th arg: Temperature (argv[6], stored as string, without modification)
									// 7th arg: XPM file #1 (argv[7])
									// 8th arg: optional, XPM file #2 (argv[8], if present and n=2)
	
	fprintf(argsfile, "%d", n);
		for (int i = 2; i < argc; i++)
	{
		fprintf(argsfile, "\"%s\"\n", argv[i]);
	}
/*
	// ----------------------------------------------------------------------------------
	if ((n != 1) && (n != 2)) return 1;		// bullet-proofing (OPUS macro is implemented for n = 1 and n = 2)
	// ----------------------------------------------------------------------------------
*/	
											// For loop to populate array of file numbers
	for (int i = 0; i < n; i++)				// This is the spectral range index
	{
		for (int j = 0; j < 2; j++)			// j=0 -> sample; j=1 -> mirror
		{
			for(int k = 0; k < 4; k++)		// enumerate pola angles
			{
				f_idx[i][k + 4 * j] = k + 4 * i + 4 * j * n;	// k for Pola angles; offset by 4i for spectral range;
			}													//  offset by 4jn to get to mirror from sample;
		}
	}
	
	for (int i = 0; i < n; i++)				// Loop to open input files
	{
		for (int j = 0; j < 8; j++)
		{
			sprintf(buffer, "%s\\%d.dat", argv[3],f_idx[i][j]);
//			ifile[i][j] = fopen(buffer, "r");
			fprintf(argsfile, "Opening %s\n", buffer);
		}
	}
	
	sprintf(buffer, "%s\\%s.ref", argv[4], argv[2]);		// Get output filename for writing
//	ofile = fopen(buffer, "w");								// Open it
	fprintf(argsfile, "Writing to %s\n", buffer);
	
//	fprintf(ofile, "Sample: %s; Temp = %s K;\nNo. of exps: %d\n", argv[5], argv[6], n);	// Write header
	
	
	
	fclose(argsfile);												// Close output file
	
	return 0;
}
