#include "Common.hpp"
#include "Strings.hpp"
#include "Compressor.hpp"


#include "system/Config.hpp"
#include "system/System.hpp"
#include "system/TextUtilities.hpp"
#include "system/Terminal.hpp"

class QuantizerConfig : public Config {
public:
	
	explicit QuantizerConfig(const std::vector<std::string> & argv) : Config(argv) {
		
		// Process arguments.
		for(const auto & arg : arguments()) {
			// Config path.
			if(arg.key == "in" || arg.key == "i"){
				inPaths = arg.values;
			}
			if(arg.key == "out" || arg.key == "o"){
				outPaths = arg.values;
			}
			if(arg.key == "mode" || arg.key == "m") {
				if(!arg.values.empty()){
					const int rawMode = std::stoi(arg.values[0]);
					if(rawMode >= 0 && rawMode < int(CompressorMode::COUNT)){
						mode = CompressorMode(rawMode);
					}
				}
			}
			if(arg.key == "colors" || arg.key == "c") {
				if(!arg.values.empty()){
					const int rawCount = std::stoi(arg.values[0]);
					colorCount = (unsigned int)std::min(256, std::max(2, rawCount));
				}
			}
			if(arg.key == "dither" || arg.key == "d") {
				dither = true;
			}
			if(arg.key == "no-alpha" || arg.key == "na") {
				noAlpha = true;
			}
			if(arg.key == "force" || arg.key == "f") {
				inPlace = true;
			}
			if(arg.key == "version" || arg.key == "v") {
				version = true;
			}
			if(arg.key == "license") {
				license = true;
			}
			if(arg.key == "license-ext") {
				licenseExt = true;
			}
			if(arg.key == "quant") {
				bonus = true;
			}
			
		}

		registerArgument("in", "i", "Image(s) to quantize", "path(s) to file(s)");
		registerArgument("out", "o", "Destination path(s) If empty, will use input path + timestamp or the input path itself if --force enabled", "path(s) to file(s)");
		registerArgument("force", "f", "Output in place if out path not specified, erasing input file(s) if present");

		registerSection("Settings");
		registerArgument("mode", "m", "Compressor to use", "(0: ImageQuant, 1: PngNeuQuant, 2: Posterizer)");
		registerArgument("colors", "c", "Number of colors in the palette", "in 2,256");
		registerArgument("dither", "d", "Should dithering be applied");
		registerArgument("no-alpha", "na", "Remove alpha channel");

		registerSection("Infos");
		registerArgument("version", "v", "Displays the current Quantizer version.");
		registerArgument("license", "", "Display the license message.");
		registerArgument("license-ext", "", "Display the dependencies license messages.");
		
	}
	
	std::vector<std::string> inPaths;
	std::vector<std::string> outPaths;
	CompressorMode mode = CompressorMode::IMAGEQUANT;
	unsigned int colorCount = 256;
	bool dither = false;
	bool noAlpha = false;
	bool inPlace = false;

	// Messages.
	bool version = false;
	bool license = false;
	bool licenseExt = false;
	bool bonus = false;
};


int main(int argc, char** argv){
	
	QuantizerConfig config(std::vector<std::string>(argv, argv+argc));
	if(config.version){
		Log::Info() << versionMessage << std::endl;
		return 0;
	} else if(config.license){
		Log::Info() << licenseMessage << std::endl;
		return 0;
	} else if(config.licenseExt){
		Log::Info() << licenseExtMessage << std::endl;
		return 0;
	} else if(config.bonus){
		Log::Info() << bonusMessage << std::endl;
		return 0;
	} else if(config.showHelp(config.inPaths.empty())){
		return 0;
	}

	int returnCode = 0;
	for(size_t iid = 0; iid < config.inPaths.size(); ++iid){

		const std::string inPath = config.inPaths[iid];
		if(inPath.empty()){
			continue;
		}

		Image refImg;
		if(!refImg.load(inPath)){
			Log::Error() << "Unable to load file at path \"" << inPath << "\"." << std::endl;
			returnCode = 1;
			continue;
		}

		// Remove alpha channel if needed.
		if(config.noAlpha){
			refImg.makeOpaque();
		}

		// Quantize.
		const Settings settings = {config.mode, config.colorCount, config.dither};

		Image outImg;
		// Returns a compressed image containing the raw output PNG data and the file size.
		bool success = Compressor::compress(refImg, settings, outImg);

		if(!success){
			// Cleanup the image now.
			refImg.clean();
			returnCode = 2;
			continue;
		}

		// Generate output path.
		std::string outPath;
		if(iid < config.outPaths.size()){
			outPath = config.outPaths[iid];
		}
		// If outpath is still empty, we either overwrite if -f, or append a timestamp.
		if(outPath.empty()){
			// Use the input as a base.
			outPath = inPath;
			TextUtilities::removeExtension(outPath);

			if(config.inPlace){
				// Overwrite.
				outPath += ".png";
			} else {
				outPath += "_" + System::timestamp() + ".png";
			}
		}
		// Save output.
		if(!outImg.save(outPath)){
			returnCode = 3;
		}
		// Log compression result
		const int percentage = (int)std::round((1.f - float(outImg.size) / float(refImg.size)) * 100.0f);
		Log::Info() << "* " << TextUtilities::fileName(inPath) << " (" << refImg.w << "x" << refImg.h << ") - " << outImg.size << " bytes (saved " << percentage << "% of " << refImg.size << " bytes)" << std::endl;

		// Cleanup.
		refImg.clean();
		outImg.clean();
	}

	return returnCode;
}
