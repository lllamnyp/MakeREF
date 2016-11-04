#include <stdio.h>
#include <string.h>

const int maxN = 2;			// Max No. of experiments

double x;					// Wavenumber
double y[8];				// Intensities
int f_idx[maxN][8];			// File index
int n = 2;

char buffer[256];

int string_trim(char *source)	// Trims a trailing backslash; path names must be terminated
{								// with spaces before closing double quote (implement in OPUS)
	char *end;
	size_t len = strlen(source);
	
	while ((len > 0) && ((source[len-1] == ' ') || (source[len-1] == '\\'))) len--;
	source[len] = '\0';

	return 0;
}

double div0(double x, double y) // Handles division by 0.0 and negative intensity values
{								// Returns 1.0 for x<0 or y<=0 as this is meaningless in reflectivity
	if (y > 0.0 && x > 0.0) return x/y; else return 1.0;
}

int main ( int argc, char *argv[] )
{
	if (argc < 8)
	{
		printf("Usage: makeref.exe n Filename \"InputPath \" \"OutputPath \" \"SampleName \" T XPM1 [XPM2]\n");
		printf("---\n");
		printf("n          Number of spectral ranges (1 or 2)\n");
		printf("Filename   Base name (without extension) of file to be saved\n");
		printf("InputPath  Path to input files (expected as: 0.dat, 1.dat, ...)\n");
		printf("OutputPath Path to output file\n");
		printf("SampleName Short description of measured sample\n");
		printf("T          Temperature\n");
		printf("XPM1       Filename for XPM of first spectral range\n");
		printf("XPM2       Filename for XPM of second spectral range (optional)\n");
		printf("---\n");
		printf("Any of the arguments containing spaces should be enclosed in double quotes. Arguments ending\n");
		printf("with a backslash \'\\\' should have a trailing space before the closing quote (especially\n");
		printf("applicable to InputPath and OutputPath.");
		return 1;
	}
	
	sscanf(argv[1], "%d", &n);		// 1st arg: No. of exps (immediately stored in n)
									// 2nd arg: filename (stored in argv[2] without modification)
	string_trim(argv[3]);			// 3rd arg: 2-col tables dir, backslash stripped
	string_trim(argv[4]);			// 4th arg: measurement dir, backslash stripped
									// 5th arg: Sample name (argv[5])
									// 6th arg: Temperature (argv[6], stored as string, without modification)
									// 7th arg: XPM file #1 (argv[7])
									// 8th arg: optional, XPM file #2 (argv[8], if present and n=2)
	
	// ----------------------------------------------------------------------------------
	if ((n != 1) && (n != 2)) return 1;		// bullet-proofing (OPUS macro is implemented for n = 1 and n = 2)
	// ----------------------------------------------------------------------------------
	
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

	FILE * ifile [n][8 * maxN];				// Declare input FILE array
	FILE * ofile;							// and output file
	
	for (int i = 0; i < n; i++)				// Loop to open input files
	{
		for (int j = 0; j < 8; j++)
		{
			sprintf(buffer, "%s\\%d.dat", argv[3],f_idx[i][j]);
			ifile[i][j] = fopen(buffer, "r");
		}
	}
	
	sprintf(buffer, "%s\\%s.ref", argv[4], argv[2]);		// Get output filename for writing
	ofile = fopen(buffer, "w");								// Open it
	
	fprintf(ofile, "Sample: %s; Temp = %s K;\nNo. of exps: %d\n", argv[5], argv[6], n);	// Write header
	
	
	for (int i = 0; i < n; i++)									// Write more header
	{
		fprintf(ofile, "Experiment %d: %s\n", i, argv[7 + i]);
	}

	for (int i = 0; i < n; i++)									// Main loop to read/write (loop over spectral ranges)
	{
		fprintf(ofile, "\n\n");									// Precede each datablock with newline
		while (fscanf(ifile[i][0], "%lf%lf", &x, &y[0]) == 2)	// While fscanf successfully obtains two elements from first file
		{														// Exectue the loop for the first datablock
			for (int j = 1; j < 8; j++)							// Already have first file, go through remaining seven
			{
				fscanf(ifile[i][j], "%*lf%lf", &y[j]);			// Get only intensity value, wn is same for all files
			}
			fprintf(ofile, " % 12.2f", x);						// Write wavenumber
			for (int j = 0; j < 8; j++)							// Loop to write 8 intensity values
			{
				fprintf(ofile, " %+12.5E", y[j]);
			}
			for (int j = 0; j < 4; j++)							// Loop to write four unnormalized reflectivity values
			{
				fprintf(ofile, " %+12.5E", div0(y[j],y[j+4]));
			}
			fprintf(ofile, "\n");								// End with newline
		}
	}
	
	for (int i = 0; i < n; i++)									// Loop to close input files
	{
		for (int j = 0; j < 8; j++)
		{
			sprintf(buffer, "%s\\%d.dat", argv[3], f_idx[i][j]);
			fclose(ifile[i][j]);
		}
	}
	
	fclose(ofile);												// Close output file

	return 0;
}
