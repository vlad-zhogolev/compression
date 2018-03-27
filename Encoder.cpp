#include "Encoder.h"

using namespace std;

void Encoder::setCode(string code, unsigned char byte)
{
    _codes[byte] = code;
}

void Encoder::encodeFile(const string& inputFileName, const string& outputFileName)
{
    _fin.open(inputFileName, ios::binary | ios::ate);
    _fout.open(outputFileName, ios::binary | ios::app); //append info, because tree is already stored
    if (_fin.is_open() && _fout.is_open())
        encodeBytes();
    _fin.close();
    _fout.close();
}

void Encoder::encodeBytes()
{
    long long bytesLeft = _fin.tellg();
    long long readBufSize = 1024 * 1024;
    long long encBufSize = readBufSize;
    int readPos = 0;
    int encPos = 0;
    int codePos = 0;
    int bitsLeft = CHAR_BIT;
    char* readBuf = new char[readBufSize];
    char* encBuf = new char[encBufSize];

    _fout << bytesLeft << 'S';  //store file size and separating symbol
    _fin.seekg(0, ios::beg);

    while (bytesLeft > 0)
    {
        readBufSize = (bytesLeft < readBufSize) ? bytesLeft : readBufSize;
        bytesLeft -= readBufSize;
        _fin.read(readBuf, readBufSize);
        readPos = 0;

        while (readPos < readBufSize)
        {
            codePos = 0;
            while (codePos < _codes[(unsigned char) readBuf[readPos]].length())
            {
                if (encPos < encBufSize)
                {   //write byte
                    encBuf[encPos] <<= 1;
                    if (_codes[(unsigned char) readBuf[readPos]][codePos] == '1')
                        encBuf[encPos] |= 1;
                    ++codePos;
                    --bitsLeft;
                    if (bitsLeft == 0)
                    {
                        ++encPos;
                        bitsLeft = CHAR_BIT;
                    }
                }
                else
                {   //write buffer
                    _fout.write(encBuf, encPos);
                    encPos = 0;
                }
            }
            ++readPos;
        }
    }
    if (bitsLeft > 0)//change for encPos
    {
        encBuf[encPos] <<= bitsLeft;
        _fout.write(encBuf, encPos + 1);
    }
    delete[] readBuf;
    delete[] encBuf;
}

//Another method for encoding using chars
/*void Encoder::encodeBytes()
{
    _fout << _fin.tellg() << 'S';  //store file size and separating symbol
    _fin.seekg(0, ios::beg);
    char ch;
    char buffer = 0;
    int spaceLeft = CHAR_BIT; //bits in char
    while (_fin.get(ch))
    {
        int codePos = 0;
        while (codePos < _codes[(unsigned char)ch].length())
        {
            buffer <<= 1;
            if ((_codes[(unsigned char)ch])[codePos] == '1')
                buffer |= 1;
#ifdef PRINT
            cout << (buffer & 1);
#endif
            ++codePos;
            --spaceLeft;
            if (spaceLeft == 0)
            {
                _fout.write(&buffer,1);
                spaceLeft = CHAR_BIT;
            }
        }
#ifdef PRINT
        cout << " ";
#endif
    }
    if (spaceLeft > 0)
    {
        buffer <<= spaceLeft;
        _fout.write(&buffer,1);
    }
}*/

//This method uses unsigned chars for encoding(original)
/*void Encoder::encodeBytes()
{
    _fout << _fin.tellg() << 'S';  //store file size and separating symbol
    _fin.seekg(0, ios::beg);
    char ch;
    unsigned char buffer = 0;
    int spaceLeft = CHAR_BIT; //bits in char
    while (_fin.get(ch))
    {
        unsigned char uch = (unsigned char) ch;
        int codePos = 0;
        while (codePos < _codes[uch].length())
        {
            buffer <<= 1;
            if ((_codes[uch])[codePos] == '1')
                buffer |= 1;
#ifdef PRINT
            cout << (buffer & 1);
#endif
            ++codePos;
            --spaceLeft;
            if (spaceLeft == 0)
            {
                _fout << buffer;
                buffer &= 0;
                spaceLeft = CHAR_BIT;
            }
        }
#ifdef PRINT
        cout << " ";
#endif
    }
    if (spaceLeft > 0)
    {
        buffer <<= spaceLeft;
        _fout << buffer;
    }
}*/