/**********************************************************************
 *                                                                    *
 * Voreen - The Volume Rendering Engine                               *
 *                                                                    *
 * Copyright (C) 2005-2010 The Voreen Team. <http://www.voreen.org>   *
 *                                                                    *
 * This file is part of the Voreen software package. Voreen is free   *
 * software: you can redistribute it and/or modify it under the terms *
 * of the GNU General Public License version 2 as published by the    *
 * Free Software Foundation.                                          *
 *                                                                    *
 * Voreen is distributed in the hope that it will be useful,          *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       *
 * GNU General Public License for more details.                       *
 *                                                                    *
 * You should have received a copy of the GNU General Public License  *
 * in the file "LICENSE.txt" along with this program.                 *
 * If not, see <http://www.gnu.org/licenses/>.                        *
 *                                                                    *
 * The authors reserve all rights not expressly granted herein. For   *
 * non-commercial academic use see the license exception specified in *
 * the file "LICENSE-academic.txt". To get information about          *
 * commercial licensing please contact the authors.                   *
 *                                                                    *
 **********************************************************************/

#include "voreen/core/datastructures/transfunc/transfuncintensity.h"
#include "voreen/core/datastructures/transfunc/transfuncmappingkey.h"
#include "voreen/core/datastructures/transfunc/transfuncfactory.h"

#ifdef VRN_WITH_DEVIL
    #include <IL/il.h>
#endif

#include "tgt/logmanager.h"

#include <tinyxml/tinyxml.h>
#include <fstream>
#include <sstream>

using tgt::col4;
using tgt::ivec3;
using tgt::ivec4;

namespace voreen {

// some stream helper functions
inline int readInt(std::ifstream& stream);
inline short readShort(std::ifstream& stream);
inline double readDouble(std::ifstream& stream);

const std::string TransFuncIntensity::loggerCat_("voreen.TransFuncIntensity");

TransFuncIntensity::TransFuncIntensity(int width)
    : TransFunc(width, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, tgt::Texture::NEAREST)
    , lowerThreshold_(0.f)
    , upperThreshold_(1.f)
{
    loadFileFormats_.push_back("tfi");
    loadFileFormats_.push_back("lut");
    loadFileFormats_.push_back("table");
    loadFileFormats_.push_back("plist");
#ifdef VRN_WITH_DEVIL
    loadFileFormats_.push_back("jpg");
    loadFileFormats_.push_back("png");
#endif

    saveFileFormats_.push_back("tfi");
    saveFileFormats_.push_back("lut");
#ifdef VRN_WITH_DEVIL
    saveFileFormats_.push_back("png");
#endif

    createStdFunc();
}

TransFuncIntensity::TransFuncIntensity(const TransFuncIntensity& tf)
    : TransFunc(tf.dimensions_.x, tf.dimensions_.y, tf.dimensions_.z,
                tf.format_, tf.dataType_, tf.filter_)
{
    updateFrom(tf);
}

TransFuncIntensity::~TransFuncIntensity() {
    for (size_t i = 0; i < keys_.size(); ++i)
        delete keys_[i];
}

bool TransFuncIntensity::operator==(const TransFuncIntensity& tf) {
    if (keys_.size() != tf.keys_.size())
        return false;
    for (size_t i = 0; i < keys_.size(); ++i) {
        if (*(keys_[i]) != *(tf.keys_[i]))
            return false;
    }

    return true;
}

bool TransFuncIntensity::operator!=(const TransFuncIntensity& tf) {
    return !(*this == tf);
}

void TransFuncIntensity::createStdFunc() {
    clearKeys();
    keys_.push_back(new TransFuncMappingKey(0.f, col4(0, 0, 0, 0)));
    keys_.push_back(new TransFuncMappingKey(1.f, col4(255)));

    textureInvalid_ = true;
}

tgt::vec4 TransFuncIntensity::getMeanValue(float segStart, float segEnd) const {
    ivec4 result(0);
    float width = static_cast<float>(tex_->getWidth());
    for (int i = static_cast<int>(segStart*width); i < segEnd*width; ++i)
        result += ivec4(tex_->texel<col4>(i));

    return static_cast<tgt::vec4>(result)/(segEnd*width-segStart*width);
}

col4 TransFuncIntensity::getMappingForValue(float value) const {
    // If there are no keys, any further calculation is meaningless
    if (keys_.empty())
        return col4(0, 0, 0, 0);

    // Restrict value to [0,1]
    value = (value < 0.f) ? 0.f : value;
    value = (value > 1.f) ? 1.f : value;

    // iterate through all keys until we get to the correct position
    std::vector<TransFuncMappingKey*>::const_iterator keyIterator = keys_.begin();

    while ((keyIterator != keys_.end()) && (value > (*keyIterator)->getIntensity()))
        keyIterator++;

    if (keyIterator == keys_.begin())
        return keys_[0]->getColorL();
    else if (keyIterator == keys_.end())
        return (*(keyIterator-1))->getColorR();
    else{
        // calculate the value weighted by the destination to the next left and right key
        TransFuncMappingKey* leftKey = *(keyIterator-1);
        TransFuncMappingKey* rightKey = *keyIterator;
        float fraction = (value - leftKey->getIntensity()) / (rightKey->getIntensity() - leftKey->getIntensity());
        col4 leftDest = leftKey->getColorR();
        col4 rightDest = rightKey->getColorL();
        col4 result = leftDest;
        result.r += static_cast<uint8_t>((rightDest.r - leftDest.r) * fraction);
        result.g += static_cast<uint8_t>((rightDest.g - leftDest.g) * fraction);
        result.b += static_cast<uint8_t>((rightDest.b - leftDest.b) * fraction);
        result.a += static_cast<uint8_t>((rightDest.a - leftDest.a) * fraction);
        return result;
    }
}

void TransFuncIntensity::updateTexture() {

    if (!tex_ || (tex_->getDimensions() != dimensions_))
        createTex();
    tgtAssert(tex_, "No texture");

    int front_end = tgt::iround(lowerThreshold_*dimensions_.x);
    int back_start = tgt::iround(upperThreshold_*dimensions_.x);
    //all values before front_end and after back_start are set to zero
    //all other values remain the same
    for (int x = 0; x < front_end; ++x)
        tex_->texel<col4>(x) = col4(0, 0, 0, 0);

    for (int x = front_end; x < back_start; ++x)
        tex_->texel<col4>(x) = col4(getMappingForValue(static_cast<float>(x) / dimensions_.x));

    for (int x = back_start; x < dimensions_.x; ++x)
        tex_->texel<col4>(x) = col4(0, 0, 0, 0);

    tex_->uploadTexture();
    LGL_ERROR;

    textureInvalid_ = false;
}

void TransFuncIntensity::setThresholds(float lower, float upper) {
    lowerThreshold_ = lower;
    upperThreshold_ = upper;
    textureInvalid_ = true;
}

void TransFuncIntensity::setThresholds(const tgt::vec2& thresholds) {
    setThresholds(thresholds.x, thresholds.y);
}

tgt::vec2 TransFuncIntensity::getThresholds() const {
    return tgt::vec2(lowerThreshold_, upperThreshold_);
}

int TransFuncIntensity::getNumKeys() const {
    return keys_.size();
}

TransFuncMappingKey* TransFuncIntensity::getKey(int i) const {
    return keys_.at(i);
}

const std::vector<TransFuncMappingKey*> TransFuncIntensity::getKeys() const {
    return keys_;
}

void TransFuncIntensity::setKeys(std::vector<TransFuncMappingKey*> keys) {
    keys_ = keys;
    textureInvalid_ = true;
}

void TransFuncIntensity::addKey(TransFuncMappingKey* key) {
    // insert key at appropriate location
    if (keys_.empty()) {
        keys_.push_back(key);
        return;
    }
    std::vector<TransFuncMappingKey *>::iterator keyIterator = keys_.begin();
    // Fast-forward to the correct position
    while ((keyIterator != keys_.end()) && (key->getIntensity() > (*keyIterator)->getIntensity()))
        keyIterator++;
    keys_.insert(keyIterator, key);

    textureInvalid_ = true;
}

bool sortFunction(TransFuncMappingKey* a, TransFuncMappingKey* b) {
    return a->getIntensity() < b->getIntensity();
}

void TransFuncIntensity::updateKey(TransFuncMappingKey* /*key*/) {
    std::sort(keys_.begin(), keys_.end(), sortFunction);

    textureInvalid_ = true;
}

void TransFuncIntensity::removeKey(TransFuncMappingKey* key) {
    std::vector<TransFuncMappingKey *>::iterator keyIterator = find(keys_.begin(), keys_.end(), key);
    if (keyIterator != keys_.end())
        keys_.erase(keyIterator);
    delete key;

    textureInvalid_ = true;
}

void TransFuncIntensity::clearKeys() {
    std::vector<TransFuncMappingKey *>::iterator keyIterator = keys_.begin();
    // First delete all the referenced objects in the heap
    while (keyIterator != keys_.end()) {
        delete (*keyIterator);
        ++keyIterator;
    }
    // then delete the entries in the vector
    keys_.clear();

    textureInvalid_ = true;
}

bool TransFuncIntensity::isEmpty() const {
    return keys_.empty();
}

bool TransFuncIntensity::save(const std::string& filename) const {
    //look for fileExtension
    std::string fileExtension;
    size_t dotPosition = filename.rfind(".");
    if (dotPosition == std::string::npos)
        return false;
    else
        fileExtension = filename.substr(dotPosition+1);

    if (fileExtension == "tfi")
        return saveTfi(filename);
    else if (fileExtension == "lut")
        return saveLUT(filename);
    else
        return saveImage(filename);
}

bool TransFuncIntensity::saveTfi(const std::string& filename) const {

    // open file stream
    std::ofstream stream(filename.c_str(), std::ios_base::out);
    if (stream.fail()) {
        LWARNING("Unable to open file " << filename << " for writing.");
        return false;
    }

    // serialize to stream
    bool success = true;
    try {
        XmlSerializer s(filename);
        s.registerFactory(TransFuncFactory::getInstance());
        s.serialize("TransFuncIntensity", this);

        s.write(stream);
        if (stream.bad()) {
            LWARNING("Unable to write to file: " << filename);
            success = false;
        }
        stream.close();
    }
    catch (SerializationException &e) {
        LWARNING("SerializationException: " << e.what());
        stream.close();
        success = false;
    }

    // log result
    if (success)
        LINFO("Saved transfer function to file: " << filename);
    else
        LWARNING("Saving transfer function failed.");

    return success;
}

bool TransFuncIntensity::saveImage(const std::string& filename) const {
    //extract file extension
    std::string fileExtension;
    size_t dotPosition = filename.rfind(".");
    fileExtension = filename.substr(dotPosition+1);

#ifdef VRN_WITH_DEVIL
    //IL does _NOT_ overwrite files by default
    ilEnable(IL_FILE_OVERWRITE);
    ILuint img;
    ilGenImages(1, &img);
    ilBindImage(img);

    int width = tex_->getWidth();
    GLubyte* im = new GLubyte[width*4];

    for (int x = 0; x < width; ++x) {
        col4 c = getMappingForValue( static_cast<float>(x)/width );
        im[(x*4)+0] = c.r;
        im[(x*4)+1] = c.g;
        im[(x*4)+2] = c.b;
        im[(x*4)+3] = c.a;
    }

    ilTexImage(width, 1, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, im);

    if (fileExtension == "png")
        ilSave(IL_PNG, (ILstring)filename.c_str());
    else {
        ilDeleteImages(1, &img);
        delete[] im;
        return false;
    }

    ilDeleteImages(1, &img);
    delete[] im;
    return true;
#else
    LERROR("Saving of " << filename  << " failed: No DevIL support.");
    return false;
#endif // VRN_WITH_DEVIL
}

bool TransFuncIntensity::saveLUT(const std::string& filename) const {
    std::fstream f;
    f.open(filename.c_str(), std::ios::out);
    for (int i = 0; i < tex_->getWidth(); ++i) {
        col4 c = getMappingForValue(static_cast<float>(i)/tex_->getWidth());
        f << static_cast<int>(c.r) << " "
          << static_cast<int>(c.g) << " "
          << static_cast<int>(c.b) << std::endl;
    }

    f.close();
    return true;
}

bool TransFuncIntensity::load(const std::string& filename) {
    // Extract the file extension
    std::string fileExtension;
    size_t dotPosition = filename.rfind(".");
    if (dotPosition != std::string::npos)
        // => the last (seperating) dot was found
        fileExtension = filename.substr(dotPosition+1);
    else
        return false;

    // switch-statement is not possible, because the compiler doesn't allow a switch by std::string
    // If further extensions shall be added, the pattern should be obvious.
    if (fileExtension == "tfi")
        return loadTfi(filename);
    if (fileExtension == "lut")
        return loadImageJ(filename);
    if (fileExtension == "table")
        return loadTextTable(filename);
    if (fileExtension == "plist")
        return loadOsirixCLUT(filename);
    // "else"
    return loadImage(filename);
}

#ifdef VRN_WITH_DEVIL
bool TransFuncIntensity::loadImage(const std::string& filename) {
    ILuint imageID;
    ILboolean success;

    // Load and bind the image
    ilGenImages(1 , &imageID);
    ilBindImage(imageID);
    success = ilLoadImage(const_cast<char*>(filename.c_str()));

    // If the load was unsuccessful => jump out of the method
    if (!success)
        return false;

    ILint imageFormat = ilGetInteger(IL_IMAGE_FORMAT);

    // Convert every color component to unsigned byte value
    success = ilConvertImage(imageFormat , IL_UNSIGNED_BYTE);

    // If the conversion was unsuccessful => jump out of the method
    if (!success)
        return false;

    // Get the width of the image file
    dimensions_.x = ilGetInteger(IL_IMAGE_WIDTH);

    // Get a pointer to the color component data. Although all rows of the image are accessible,
    // we'll need only the first row. The rest will be ignored
    ILubyte* imageData = ilGetData();

    if (imageFormat == IL_RGB) {
        // If there is no alpha channel in the file, we have to add an alpha value of 255 at every
        // fourth position. In order to do this, we'll go through both arrays at different paces
        // and add the required 255 in the 'newData' array
        ILubyte* newImageData = new ILubyte[dimensions_.x*4];
        for (int i = 0; i < dimensions_.x; ++i) {
            newImageData[i*4 + 0] = imageData[i*3 + 0];
            newImageData[i*4 + 1] = imageData[i*3 + 1];
            newImageData[i*4 + 2] = imageData[i*3 + 2];
            newImageData[i*4 + 3] = (char)(255);
        }
        imageData = newImageData;
    }
    LINFO("opened transfer function: " << filename);

    // Generate the keys from the given data
    generateKeys(reinterpret_cast<unsigned char*>(imageData));

    // Little cleanup
    ilDeleteImages(1, &imageID);

    // If we got up to here, everything was fine
    return true;
#else
bool TransFuncIntensity::loadImage(const std::string&) {
    LERROR("Loading failed: No DevIL support.");
    return false;
#endif // VRN_WITH_DEVIL
}

namespace {

// Converter for old .tfi format.
// In case the stream contains an old-style .tfi format, a version converted to the new XML is
// returned as a string, or an empty string otherwise.
std::string convertOldTfi(std::ifstream& stream) {
    std::ostringstream out;

    TiXmlDocument doc;
    std::stringbuf buffer;
    do {
        // Use 0 character instead of '\n' to minimize the number of get-calls...
        stream.get(buffer, 0);
    } while (stream.good() && !stream.eof()
             && (buffer.sputc(stream.get()) != std::stringbuf::traits_type::eof()));

    stream.seekg(0); // rewind
    stream.clear();  // and clear error state

    doc.Parse(buffer.str().c_str());
    TiXmlElement* root = doc.RootElement();

    if (!root || (root->ValueStr() != "TransFuncIntensity" && root->ValueStr() != "TransFuncIntensityKeys"))
        return "";

    out << "<?xml version=\"1.0\" ?>\n" << "<VoreenData version=\"1\">\n"
        << "<TransFuncIntensity type=\"TransFuncIntensity\">\n" << "<Keys>\n";

    for (TiXmlNode* key = root->FirstChild("key"); key != 0; key = key->NextSibling("key")) {
        out << "<key type=\"TransFuncMappingKey\">\n";

        TiXmlElement* elem = key->ToElement();
        if (elem) {
            const char* intensity = elem->Attribute("intensity");
            if (intensity)
                out << "<intensity value=\"" << intensity << "\" />\n";

            int split = 0;
            elem->QueryIntAttribute("split", &split);
            if (split == 0)
                out << "<split value=\"false\" />\n";
            else
                out << "<split value=\"true\" />\n";

            int count = 0;
            for (TiXmlNode* col = elem->FirstChild("col4"); col != 0; col = col->NextSibling("col4")) {
                count++;
                TiXmlElement* col_elem = col->ToElement();
                if (col_elem) {
                    int r = 0;
                    int g = 0;
                    int b = 0;
                    int a = 0;
                    col_elem->QueryIntAttribute("r", &r);
                    col_elem->QueryIntAttribute("g", &g);
                    col_elem->QueryIntAttribute("b", &b);
                    col_elem->QueryIntAttribute("a", &a);

                    out << "<" << (count == 1 ? "colorL" : "colorR") << " "
                        << "r=\"" << r << "\" g=\"" << g << "\" b=\"" << b << "\" a=\"" << a << "\" />\n";
                }
            }
        }
        out << "</key>\n";
    }
    out << "</Keys>\n" << "<lower value=\"0.0\" />\n" << "<upper value=\"1.0\" />\n"
        << "</TransFuncIntensity>\n" << "</VoreenData>\n";

    return out.str();
}

} // namespace

bool TransFuncIntensity::loadTfi(const std::string& filename) {
    // open file stream
    std::ifstream stream(filename.c_str(), std::ios_base::in);
    if (stream.fail()) {
        LWARNING("Unable to open file " << filename << " for reading.");
        return false;
    }

    // Convert XML to new format if necessary.
    std::string converted = convertOldTfi(stream);
    if (!converted.empty())
        LINFO("Converted transfer function from old file format: " << filename);

    // deserialize from stream
    bool success = true;
    try {
        XmlDeserializer d(filename);
        d.registerFactory(TransFuncFactory::getInstance());

        if (converted.empty())
            d.read(stream);
        else {
            std::istringstream str(converted);
            d.read(str);
        }

        d.deserialize("TransFuncIntensity", *this);
        stream.close();
    }
    catch (SerializationException &e) {
        LWARNING("SerializationException: " << e.what());
        stream.close();
        success = false;
    }

    // log result
    if (success)
        LINFO("Loaded transfer function from file: " << filename);
    else
        LWARNING("Loading transfer function failed.");

    invalidateTexture();
    return success;
}

void TransFuncIntensity::updateFrom(const TransFuncIntensity& tf) {

    lowerThreshold_ = tf.lowerThreshold_;
    upperThreshold_ = tf.upperThreshold_;

    clearKeys();
    for (size_t i = 0; i < tf.keys_.size(); ++i) {
        TransFuncMappingKey* k = new TransFuncMappingKey(*(tf.keys_.at(i)));
        addKey(k);
    }

}

void TransFuncIntensity::serialize(XmlSerializer& s) const {

    TransFunc::serialize(s);

    // serialize keys...
    s.serialize("Keys", keys_, "key");

    // serialize thresholds...
    s.serialize("lower", lowerThreshold_);
    s.serialize("upper", upperThreshold_);
}

void TransFuncIntensity::deserialize(XmlDeserializer& s) {

    TransFunc::deserialize(s);

    // deserialize keys...
    s.deserialize("Keys", keys_, "key");

    // deserialize thresholds...
    s.deserialize("lower", lowerThreshold_);
    s.deserialize("upper", upperThreshold_);
}

bool TransFuncIntensity::loadTextTable(const std::string& filename) {
    if (filename == "")
        return false;

    FILE *file = fopen(filename.c_str(), "rt");

    if (file) {
        unsigned char* data = new unsigned char[256*4];
        for (int i = 0; i < 256; ++i) {
            int _red = 0, _green = 0, _blue = 0, _alpha = 0;
            int result = fscanf(file, "%d %d %d %d", &_red, &_green, &_blue, &_alpha);
            if (result != 4)
                LWARNING("loadTextTable(): invalid line");
            data[4*i + 0] = _red;
            data[4*i + 1] = _green;
            data[4*i + 2] = _blue;
            data[4*i + 3] = _alpha;
        }
        fclose(file);
        LINFO("opened transfer function: " << filename);
        dimensions_ = tgt::ivec3(256, 1, 1);
        generateKeys(data);
        delete[] data;

        return true;
    }
    else {
        fclose(file);
        LWARNING("unable to open transfer function: " << filename);
        return false;
    }
}

bool TransFuncIntensity::loadOsirixCLUT(const std::string& filename) {
    LINFO("Opening Osirix CLUT: " << filename);

    TiXmlDocument doc(filename.c_str());

    //TODO: this loader is much to specific to a certain order of XML tags and will crash if
    // anything is missing (#276).
    if (doc.LoadFile()) {
        // read and check version of plist file
        TiXmlNode* currNode = doc.FirstChild("plist");
        TiXmlElement* currElement = currNode->ToElement();

        currNode = currNode->FirstChild("dict");
        currNode = currNode->FirstChild("key");
        currElement = currNode->ToElement();

        // get reference to red, green and blue channel
        TiXmlElement* blueElement = 0;
        TiXmlElement* greenElement = 0;
        TiXmlElement* redElement = 0;
        TiXmlNode* blueNode = currElement->NextSibling();
        TiXmlNode* greenNode = ((blueNode->NextSibling())->NextSibling());
        TiXmlNode* redNode = ((greenNode->NextSibling())->NextSibling());
        blueNode = blueNode->FirstChild("integer");
        greenNode = greenNode->FirstChild("integer");
        redNode = redNode->FirstChild("integer");

        unsigned char* data = new unsigned char[256*4];

        for (int i = 0; i < 256; ++i) {
            data[4*i + 0] = 0;
            data[4*i + 1] = 0;
            data[4*i + 2] = 0;
            data[4*i + 3] = 0;

            blueNode = blueNode->NextSibling("integer");
            greenNode = greenNode->NextSibling("integer");
            redNode = redNode->NextSibling("integer");

            if (blueNode == 0 || greenNode == 0 || redNode == 0)
                continue;

            blueElement = blueNode->ToElement();
            greenElement = greenNode->ToElement();
            redElement = redNode->ToElement();

            data[4*i + 0] = atoi(redElement->GetText());
            data[4*i + 1] = atoi(greenElement->GetText());
            data[4*i + 2] = atoi(blueElement->GetText());
            data[4*i + 3] = (char)(255);
        }

        dimensions_ = tgt::ivec3(256, 1, 1);
        generateKeys(data);
        delete[] data;

        return true;
    }
    else
        return false;
}

bool TransFuncIntensity::loadImageJ(const std::string& filename) {
    std::ifstream fileStream(filename.c_str(), std::ios::in|std::ios::binary);

    if (fileStream.good()) {
        // Determine the length of file; necessary for typeswitch later
        size_t length;
        fileStream.seekg(0, std::ios::end);
        length = fileStream.tellg();
        fileStream.seekg(std::ios::beg);

        // If the length is that high, we can't use the data anyway
        if (length > 10000) {
            LWARNING("unable to open transfer function: " << filename);
            return false;
        }

        // The following procedures return 0 if the loading attempt was without success
        int size = 0;
        if (length > 768)
            // load NIH Image LUT
            size = openImageJBinary(fileStream, false);
        if ((size == 0) && ((length == 0) || (length == 768) || (length == 970)))
            //load raw LUT
            size = openImageJBinary(fileStream, true);
        if ((size == 0) && (length >= 768))
            size = openImageJText(fileStream);
        if (size == 0) {
            LWARNING("unable to open transfer function: " << filename);
        }

        // if size = 256, at least one of the procedures above was successful
        return (size == 256);

    }
    else {
        // The file couldn't be opened
        LWARNING("unable to open transfer function: " << filename);
        return false;
    }
}

int TransFuncIntensity::openImageJBinary(std::ifstream& fileStream, bool raw) {
    // the default quantity of colors
    int numColors = 256;
    if (!raw) {
        // read the header information seperatly
        int id = readInt(fileStream);
        if (id != 1229147980) {
            // a leading 1229147980 (= 'ICOL') indicates an NIH Image LUT
            // if ICOL isn't the first entry in the table, let the other procedures handle it
            // but first, go back to the beginning
            fileStream.seekg(std::ios::beg);
            return 0;
        }
        readShort(fileStream); // Version
        numColors = readShort(fileStream); // Number of Colors
        readShort(fileStream);  // Start
        readShort(fileStream);  // End
        readDouble(fileStream); // Filler1
        readDouble(fileStream); // Filler2
        readInt(fileStream);    // Filler3
    }

    // The colors in a binary table are saved in succession so
    // first load the reds, then greens and at last blues
    char redColors[256];
    char greenColors[256];
    char blueColors[256];
    try {
        fileStream.read(&redColors[0], numColors);
        fileStream.read(&greenColors[0], numColors);
        fileStream.read(&blueColors[0], numColors);
    }
    catch (...) {
        throw;
    }

    unsigned char data[256*4];

    for (int i = 0; i < 256; ++i) {
        data[i*4 + 0] = redColors[i];
        data[i*4 + 1] = greenColors[i];
        data[i*4 + 2] = blueColors[i];
        data[i*4 + 3] = (char)(255);
    }

    dimensions_ = tgt::ivec3(256, 1, 1);
    generateKeys(&data[0]);

    return 256;
}

int TransFuncIntensity::openImageJText(std::ifstream& fileStream){
    // in this array, the converted values are stored
    // The maximum are 256*4 entries
    unsigned char* data = new unsigned char[256*4];
    int tmp;
    int numValues = 0;
    char* entry = new char[50];
    int strToIntConversionStatus = 0;

    while (strToIntConversionStatus != -1) {
        fileStream >> entry;

        // After this line StrToIntConversionStatus is:
        // 1 if 'line' was an integer ; 0 otherwise ; -1 if the end of the sfile is reached
        strToIntConversionStatus = sscanf(entry, "%i", &tmp);

        // If an integer is encountered, add it to the array and increase the number of values
        if (strToIntConversionStatus == 1) {
            data[numValues] = tmp;
            ++numValues;
        }
    }
    delete[] entry;

    /*  Now, two values of 'numValues' are possible. 256*3 = 768 or 256*4 = 1024.
        768, if there were 3 entries in a row
        1024, if there were 4 entires
    */
    if ((numValues != 768) && (numValues != 1024)) {
        // A wrong file was loaded
        delete[] data;
        return 0;
    }

    if (numValues == 1024) {
        // The first entry is an index, therefore it can be overwritten
        // Additional to that, an alpha value of 255 is inserted at the 4th position
        for (int i = 0; i < 256; ++i) {
            data[i*4 + 0] = data[i*4 + 1];
            data[i*4 + 1] = data[i*4 + 2];
            data[i*4 + 2] = data[i*4 + 3];
            data[i*4 + 3] = (char)(255);
        }

        dimensions_ = tgt::ivec3(256, 1, 1);
        generateKeys(data);
    }
    else {
        /* This one is a little bit tricky. We had 768 values in the file and added them
        sequentially to the data-array. Now we have to insert a '255' at every fourth position.
        In order to do that, we create a new array and move through both arrays at different
        paces.
        */
        unsigned char* newData = new unsigned char[1024];

        for (int i = 0; i < 256; ++i) {
            newData[i*4 + 0] = data[i*3 + 0];
            newData[i*4 + 1] = data[i*3 + 1];
            newData[i*4 + 2] = data[i*3 + 2];
            newData[i*4 + 3] = (char)(255);
        }

        dimensions_ = tgt::ivec3(256, 1, 1);
        generateKeys(newData);
        delete[] newData;
    }

    delete[] data;
    return 256;
}

void TransFuncIntensity::generateKeys(unsigned char* data) {
    /* A short overview about the idea behind this method. For the sake of simplicity this is
    demonstrated with only one color channel, say 'Red'. It is generalized in the code below:

    We want to detect the peaks(=extrema) in the graph containing all the 'red'-values.
    In order to do this, we look at one 1/width-th at a time and compare the difference between
    the (i-2)th and (i-1)th point (= oldDelta_x) to the difference between the (i-1)th and ith point
    (stored in newDelta_x). Several possible things can happen:
    i) The difference doesn't change at all. This means that the difference between the points is
       linearly dependent and in this case, we don't have to add another key because we get linear
       interpolation from the the methods
    ii) The difference might be not-zero. This means the graph is discontinuous at this point and
        we have to insert a splitted point. On the left side we take the color from the
        (i-1)th point and on the right side from the ith point. The discontinuity is represented
        by the "jump" between the two parts of the splitted mapping key.
    iii) The difference could have changed by a factor of -1. In this case, we have to insert a
         mapping key right at the peak (i.e. at the ith point).

    If a key is placed, we don't want to place a key at the next location, because the difference
    will change not matter if there is a peak or not. There are a lot of cases, in this
    redundant keys will be generated and we don't want that


    In the code below, we generate a key whenever any of the colorchannels meets a criterion above.
    */

    // Storage for the old values
    int oldDeltaRed;
    int oldDeltaGreen;
    int oldDeltaBlue;
    int oldDeltaAlpha;

    // Storage for the new values
    int newDeltaRed;
    int newDeltaGreen;
    int newDeltaBlue;
    int newDeltaAlpha;

    // We want at least 2 values in the data array
    if (dimensions_.x < 2)
        return;

    clearKeys();

    addKey(new TransFuncMappingKey(0.f,
           col4(data[0], data[1], data[2], data[3])));
    addKey(new TransFuncMappingKey(1.f,
           col4(data[4*(dimensions_.x-1)+0], data[4*(dimensions_.x-1)+1], data[4*(dimensions_.x-1)+2], data[4*(dimensions_.x-1)+3])));

    // Calculate the starting point
    newDeltaRed   = data[4*1 + 0] - data[4*0 + 0];
    newDeltaGreen = data[4*1 + 1] - data[4*0 + 1];
    newDeltaBlue  = data[4*1 + 2] - data[4*0 + 2];
    newDeltaAlpha = data[4*1 + 3] - data[4*0 + 3];

    // The main loop. We start at 2 because the value for 1 already has been calculated.
    for (int iter = 2; iter < dimensions_.x; ++iter) {
        // Backup the old values and generate the new ones.
        oldDeltaRed = newDeltaRed;
        oldDeltaGreen = newDeltaGreen;
        oldDeltaBlue = newDeltaBlue;
        oldDeltaAlpha = newDeltaAlpha;

        newDeltaRed   = data[4*iter + 0] - data[4*(iter-1) + 0];
        newDeltaGreen = data[4*iter + 1] - data[4*(iter-1) + 1];
        newDeltaBlue  = data[4*iter + 2] - data[4*(iter-1) + 2];
        newDeltaAlpha = data[4*iter + 3] - data[4*(iter-1) + 3];

        // Has the difference quotient changed in any color channel?
        bool differenceQuotientChanged = (
            (oldDeltaRed   != newDeltaRed)   ||
            (oldDeltaGreen != newDeltaGreen) ||
            (oldDeltaBlue  != newDeltaBlue)  ||
            (oldDeltaAlpha != newDeltaAlpha));

        // Is the difference quotient different from zero in any channel?
        bool differenceQuotientNotZero = (
            (newDeltaRed   != 0) ||
            (newDeltaGreen != 0) ||
            (newDeltaBlue  != 0) ||
            (newDeltaAlpha != 0));

        // Has the difference quotient tilted in all channel's?
        // Mind the & instead of |
        bool differenceQuotientTilted = (
            (oldDeltaRed   == -newDeltaRed)   &&
            (oldDeltaGreen == -newDeltaGreen) &&
            (oldDeltaBlue  == -newDeltaBlue)  &&
            (oldDeltaAlpha == -newDeltaAlpha));

        if (differenceQuotientChanged) {
            if (differenceQuotientNotZero) {
                // We want to put a splitted key here (see ii above)
                TransFuncMappingKey* newkey = new TransFuncMappingKey(iter/static_cast<float>(dimensions_.x-1) ,
                    col4( data[4*(iter-1) + 0], data[4*(iter-1) + 1], data[4*(iter-1) + 2], data[4*(iter-1) + 3] )
                    );
                newkey->setSplit(true);
                newkey->setColorR(tgt::col4(data[4*iter + 0], data[4*iter + 1], data[4*iter + 2], data[4*iter + 3]));
                addKey(newkey);
            }
            else if (differenceQuotientTilted) {
                // We want a single key at i-1 here (see iii above)
                addKey(
                    new TransFuncMappingKey((iter - 1)/static_cast<float>(dimensions_.x-1),
                    col4(data[4*(iter-1) + 0], data[4*(iter-1) + 1], data[4*(iter-1) + 2], data[4*(iter-1) + 3])
                    ));
            }
            else {
                // Just add a key
                addKey(
                    new TransFuncMappingKey(iter/static_cast<float>(dimensions_.x-1),
                    col4(data[4*iter + 0], data[4*iter + 1], data[4*iter + 2], data[4*iter + 3])));
            }
        }
    }
}

// --------------------------------------------------------------------------------
// some stream helper functions

/**
 * Used not only to extract neccessary information but also to fast-forward through the header
 * Buffersize is given static, because the procedure shouldn't be edited if 64bit compiler is used
 * (and therefor the sizes i.e. int, double might change)
 * These functions are 'inline', because it might be faster if they are inlined by the compiler
 * (okay, not quite _that_ convincing i admit).
 *
 * @param stream the already opened file stream used to extract the data
 * @return the read int value
 */
inline int readInt(std::ifstream& stream) {
    char* buffer = new char[4];

    // The bytes are inserted backwards because of a discrepancy of
    // most significant bit <-> least significant bit between Java and C++
    for (int i = 3; i >= 0; --i)
        stream >> buffer[i];

    int ret = *(reinterpret_cast<int*>(buffer));
    delete[] buffer;
    return ret;
}

/**
 * \sa TransFuncIntensity::readInt(std::ifstream& stream)
 */
inline short readShort(std::ifstream& stream) {
    char* buffer = new char[2];

    for (int i = 1; i >= 0; --i)
        stream >> buffer[i];

    short ret = *(reinterpret_cast<short*>(buffer));
    delete[] buffer;
    return ret;
}

/**
 * \sa TransFuncIntensity::readInt(std::ifstream& stream)
 */
inline double readDouble(std::ifstream& stream) {
    char* buffer = new char[8];

    for (int i = 7; i >= 0; --i)
        stream >> buffer[i];

    double ret = *(reinterpret_cast<double*>(buffer));
    delete[] buffer;
    return ret;
}

TransFunc* TransFuncIntensity::clone() const {
    TransFuncIntensity* func = new TransFuncIntensity();

    func->dimensions_.x = dimensions_.x;
    func->dimensions_.y = dimensions_.y;
    func->dimensions_.z = dimensions_.z;
    func->format_ = format_;
    func->dataType_ = dataType_;
    func->filter_ = filter_;

    func->lowerThreshold_ = lowerThreshold_;
    func->upperThreshold_ = upperThreshold_;

    func->keys_.clear();
    std::vector<TransFuncMappingKey*>::const_iterator it;
    for(it = keys_.begin(); it!=keys_.end(); it++) {
        func->keys_.push_back((*it)->clone());
    }

    func->textureInvalid_ = true;

    return func;
}

} // namespace voreen
