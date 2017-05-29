#include <iostream>
#include <memory>
#include <grale/plummerlens.h>
#include <grale/masssheetlens.h>
#include <grale/compositelens.h>
#include <grale/constants.h>

using namespace std;
using namespace grale;

int ProcessCompositeLens(const CompositeLens *pCompLens, Vector2D<double> coordOffset, double scaleFactor)
{
	int numLenses = pCompLens->getNumberOfSubLenses();
	for (int i = 0 ; i < numLenses ; i++)
	{
		const GravitationalLens *pSubLens = pCompLens->getSubLens(i);
		const PlummerLens *pPlummerLens = dynamic_cast<const PlummerLens *>(pSubLens);
		const MassSheetLens *pSheetLens = dynamic_cast<const MassSheetLens *>(pSubLens);
		const CompositeLens *pNewCompLens = dynamic_cast<const CompositeLens *>(pSubLens);
		Vector2D<double> subLensPos = pCompLens->getSubLensPosition(i);
		double subLensFactor = pCompLens->getSubLensFactor(i);

		if (pPlummerLens)
		{
			const PlummerLensParams *pParams = static_cast<const PlummerLensParams *>(pPlummerLens->getLensParameters());
			double mass = pParams->getLensMass() * scaleFactor * subLensFactor;
			double width = pParams->getAngularWidth();
			Vector2D<double> pos = subLensPos;
			pos += coordOffset;

			cout << "Plummer: center=" << pos.getX()/ANGLE_ARCSEC << "," << pos.getY()/ANGLE_ARCSEC
				 << " width=" << width/ANGLE_ARCSEC << " mass=" << mass/MASS_SOLAR << endl;
		}
		else if (pSheetLens)
		{
			const MassSheetLensParams *pParams = static_cast<const MassSheetLensParams *>(pSheetLens->getLensParameters());
			double dens = pParams->getDensity() * scaleFactor * subLensFactor;
			
			cout << "Sheet: density=" << dens << endl;
		}
		else if (pNewCompLens)
		{
			int status = ProcessCompositeLens(pNewCompLens, coordOffset + subLensPos, scaleFactor*subLensFactor);
			if (status != 0)
				return status;
		}
		else
		{
			cerr << "Sublens is neither a Plummer lens,  a mass-sheet lens or composite lens (type is " << (int)pSubLens->getLensType() << endl;
			return -1;
		}
	}

	return 0;
}

int main(int argc, char *argv[])
{
	string info(R"XYZ(# For Plummer lens: position and width are in units of arcsec, mass in units 
# of solar mass
# For mass sheet: density unit is kg m^-2)XYZ");

	if (argc != 2)
	{
		cerr << "Usage: " << argv[0] << " lensfile.lensdata" << endl << endl;
		cerr << info << endl << endl;
		return -1;
	}

	string fileName(argv[1]);
	GravitationalLens *pLens = nullptr;
	string errStr;

	if (!GravitationalLens::load(fileName, &pLens, errStr))
	{
		cerr << "Unable to load lens '" << fileName << "': " << errStr << endl;
		return -1;
	}

	unique_ptr<GravitationalLens> lens(pLens); // to clean up automatically
	CompositeLens *pCompLens = dynamic_cast<CompositeLens *>(pLens);
	if (pCompLens == 0)
	{
		cerr << "Not a composite lens" << endl;
		return -1;
	}

	cout.precision(17);
	cout << info << endl;

	return ProcessCompositeLens(pCompLens, Vector2D<double>(0, 0), 1.0);
}
