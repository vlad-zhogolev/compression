/**
 * КДЗ по дисциплине Алгоритмы и Структуры Данных 2017-2018 уч.год
 * Жоголев Владислав Романович, группа БПИ-164, дата 26.03.2018
 * Среда разработки: CLion 2017.3.1
 *
 * Состав проекта:
 * Huffman.h
 * Huffman.cpp
 * ShannonFano.h
 * ShannonFano.cpp
 * Encoder.h
 * Encoder.cpp
 * Decoder.h
 * Decoder.cpp
 * Node.h
 * Node.cpp
 * LZ77.h
 * LZ77.cpp
 * LZ77Decoder.h
 * LZ77Decoder.cpp
 *
 * Реализованы следующие алгоритмы сжатия данных без потерь,
 * для упаковки и распаковки файлов различного типа:
 * 1) алгоритм Хаффмана
 * 2) алгоритм Шеннона-Фано
 * 3) алгоритм Лемпелля-Зива 77 года (все 3 варианта)
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>
#include "Huffman.h"
#include "ShannonFano.h"
#include "LZ77.h"
#include "LZ77Decoder.h"
#include "Encoder.h"
#include "Decoder.h"

using namespace std;
using namespace chrono;

const int FILES_NUMBER = 36;
const int TESTS_NUMBER = 20;
const int SYMBOLS_NUMBER = UCHAR_MAX + 1;
int frequences[SYMBOLS_NUMBER];
string inPath = "samples-for-students/";
string comprPath = "compressed/";
string decomprPath = "decompressed/";
string csvPath = "csv/";
string freqPath = "frequences/";
string coefPath = "coefficients/";
string huffmanPath = "Huffman/";
string shannonFanoPath = "ShannonFano/";
string lz775Path = "LZ775/";
string lz7710Path = "LZ7710/";
string lz7720Path = "LZ7720/";

//reads file as binary and stores frequences in provided array (it must have SYMBOLS_NUMBER elements)
void countFrequencesFromFile(string filename, int* frequences)
{
    ifstream fin;
    fin.open(filename, ios::binary);
    if (fin.is_open())
    {
        for (int i = 0; i < SYMBOLS_NUMBER; ++i)
            frequences[i] = 0;

        char ch;
        while (fin.get(ch))
            ++frequences[(unsigned char) ch];  //cast to get index, should be used to access the right frequency
    }

    fin.close();
}

//function for comparing pairs in vector
bool byFreq(const pair<int, unsigned char>& pair1, const pair<int, unsigned char>& pair2)
{
    return pair1.first > pair2.first;
}

//stores all pairs with non-zero frequences in vector
vector<pair<int, unsigned char>> sortSymbolsByFrequences(int* frequences)
{
    vector<pair<int, unsigned char>> freqTable;
    for (int i = 0; i < SYMBOLS_NUMBER; ++i)
    {
        if (frequences[i] != 0)
            freqTable.emplace_back(frequences[i], (unsigned char) i);
    }
    sort(freqTable.begin(), freqTable.end(), byFreq);
    return freqTable;
};

long long getFileSize(const string& filename)
{
    ifstream fin;
    fin.open(filename, ios::ate);
    long long fileSize = fin.tellg();
    fin.close();
    return fileSize;
}

string doubleToCSV(double value)
{
    string res = to_string(value);
    for (int i = 0; i < res.length(); ++i)
        if (res[i] == '.')
            res[i] = ',';
    return res;
}

void calcEntropies()
{
    ofstream csv;

    csv.open(csvPath + "Entropies.csv");
    for (int fileNum = 1; fileNum <= FILES_NUMBER; ++fileNum)
    {
        string number = ((fileNum >= 10) ? "" : "0") + to_string(fileNum);
        string input = inPath + number;
        countFrequencesFromFile(input, frequences);
        vector<pair<int, unsigned char>> sortedFrequences = sortSymbolsByFrequences(frequences);
        long long fileSize = getFileSize(input);
        double entropy = 0;
        for (int i = sortedFrequences.size() - 1; i >= 0; --i)
            entropy += -((sortedFrequences[i].first) / (double) fileSize) *
                       log2((sortedFrequences[i].first) / (double) fileSize);
        csv << fileNum << ";" << doubleToCSV(entropy) << ";" << endl;
    }
    csv.close();
}

void calcFrequences()
{
    ofstream csv;

    for (int fileNum = 1; fileNum <= FILES_NUMBER; ++fileNum)
    {
        string number = ((fileNum >= 10) ? "" : "0") + to_string(fileNum);
        string input = inPath + number;
        csv.open(csvPath + freqPath + "freq" + number + ".csv");
        countFrequencesFromFile(input, frequences);

        long long fileSize = getFileSize(input);

        for (int i = 0; i <= UCHAR_MAX; ++i)
            csv << i << ";" << doubleToCSV(((double) frequences[i]) / fileSize) << endl;

        csv.close();
    }
}

void calcCompressionCoeffs()
{
    ofstream huffmanCoeffs;
    ofstream shannonFanoCoeffs;
    ofstream lz775Coeffs;
    ofstream lz7710Coeffs;
    ofstream lz7720Coeffs;

    huffmanCoeffs.open(csvPath + coefPath + "HuffmanCoeffs.csv");
    shannonFanoCoeffs.open(csvPath + coefPath + "ShannonFanoCoeffs.csv");
    lz775Coeffs.open(csvPath + coefPath + "LZ775Coeffs.csv");
    lz7710Coeffs.open(csvPath + coefPath + "LZ7710Coeffs.csv");
    lz7720Coeffs.open(csvPath + coefPath + "LZ7720Coeffs.csv");

    for (int fileNum = 1; fileNum <= FILES_NUMBER; ++fileNum)
    {
        string number = ((fileNum >= 10) ? "" : "0") + to_string(fileNum);
        string input = inPath + number;
        string huffman = comprPath + huffmanPath + number + ".huff";
        string shannonFano = comprPath + shannonFanoPath + number + ".shan";
        string lz775 = comprPath + lz775Path + number + ".lz775";
        string lz7710 = comprPath + lz7710Path + number + ".lz7710";
        string lz7720 = comprPath + lz7720Path + number + ".lz7720";

        long long fileSize = getFileSize(input);
        long long huffmanSize = getFileSize(huffman);
        long long shannonFanoSize = getFileSize(shannonFano);
        long long lz775Size = getFileSize(lz775);
        long long lz7710Size = getFileSize(lz7710);
        long long lz7720Size = getFileSize(lz7720);

        huffmanCoeffs << fileNum << ";" << doubleToCSV(((double) huffmanSize) / fileSize) << endl;
        shannonFanoCoeffs << fileNum << ";" << doubleToCSV(((double) shannonFanoSize) / fileSize) << endl;
        lz775Coeffs << fileNum << ";" << doubleToCSV(((double) lz775Size) / fileSize) << endl;
        lz7710Coeffs << fileNum << ";" << doubleToCSV(((double) lz7710Size) / fileSize) << endl;
        lz7720Coeffs << fileNum << ";" << doubleToCSV(((double) lz7720Size) / fileSize) << endl;
    }

    huffmanCoeffs.close();
    shannonFanoCoeffs.close();
    lz775Coeffs.close();
    lz7710Coeffs.close();
    lz7720Coeffs.close();
}

void huffmanTest()
{
    ofstream csv;
    steady_clock::time_point startTime;
    steady_clock::time_point endTime;

    csv.open(csvPath + "Huffman.csv");
    for (int fileNum = 34; fileNum <= FILES_NUMBER; ++fileNum)
    {
        cout << "Huffman " << fileNum << endl;
        try
        {
            string number = ((fileNum >= 10) ? "" : "0") + to_string(fileNum);
            string input = inPath + number;
            string compressed = comprPath + huffmanPath + number + ".huff";
            string decompressed = decomprPath + huffmanPath + number + ".unhaff";
            nanoseconds comprTime{};
            nanoseconds decomprTime{};

            csv << fileNum << ";";


            for (int i = 0; i < TESTS_NUMBER; ++i)
            {
                Encoder encoder;
                Decoder decoder;
                Huffman huffman;
                vector<pair<int, unsigned char>> sortedFrequences;

                startTime = steady_clock::now();
                countFrequencesFromFile(input, frequences);
                sortedFrequences = sortSymbolsByFrequences(frequences);
                for (int i = 0; i < sortedFrequences.size(); ++i)
                    huffman.addChance(sortedFrequences[i].first, sortedFrequences[i].second);
                huffman.buildTree();
                huffman.writeTreeToFile(compressed);
                for (int i = 0; i < sortedFrequences.size(); ++i)
                    encoder.setCode(huffman.get(i), sortedFrequences[i].second);
                encoder.encodeFile(input, compressed);
                endTime = steady_clock::now();

                comprTime += duration_cast<nanoseconds>(endTime - startTime);

                startTime = steady_clock::now();
                decoder.decode(compressed, decompressed);
                endTime = steady_clock::now();

                decomprTime += duration_cast<nanoseconds>(endTime - startTime);
            }
            csv << doubleToCSV(((double) comprTime.count()) / TESTS_NUMBER) << ";"
                << doubleToCSV(((double) decomprTime.count()) / TESTS_NUMBER) << endl;
        }
        catch (exception& ex)
        {
            cout << ex.what() << endl;
        }
    }
    csv.close();
}

void shannonFanoTest()
{
    ofstream csv;
    steady_clock::time_point startTime;
    steady_clock::time_point endTime;

    csv.open(csvPath + "ShannonFano.csv");
    for (int fileNum = 1; fileNum <= FILES_NUMBER; ++fileNum)
    {
        string number = ((fileNum >= 10) ? "" : "0") + to_string(fileNum);
        string input = inPath + number;
        string compressed = comprPath + shannonFanoPath + number + ".shan";
        string decompressed = decomprPath + shannonFanoPath + number + ".unshan";
        nanoseconds comprTime{};
        nanoseconds decomprTime{};

        csv << fileNum << ";";

        for (int i = 0; i < TESTS_NUMBER; ++i)
        {
            Encoder encoder;
            Decoder decoder;
            ShannonFano shannonFano;
            vector<pair<int, unsigned char>> sortedFrequences;

            startTime = steady_clock::now();
            countFrequencesFromFile(input, frequences);
            sortedFrequences = sortSymbolsByFrequences(frequences);
            for (int j = 0; j < sortedFrequences.size(); ++j)
                shannonFano.addChance(sortedFrequences[j].first, sortedFrequences[j].second);
            shannonFano.buildTree();
            shannonFano.writeTreeToFile(compressed);
            for (int j = 0; j < sortedFrequences.size(); ++j)
                encoder.setCode(shannonFano.get(j), sortedFrequences[j].second);
            encoder.encodeFile(input, compressed);
            endTime = steady_clock::now();

            comprTime += duration_cast<nanoseconds>(endTime - startTime);

            startTime = steady_clock::now();
            decoder.decode(compressed, decompressed);
            endTime = steady_clock::now();

            decomprTime += duration_cast<nanoseconds>(endTime - startTime);
        }

        csv << doubleToCSV(((double) comprTime.count()) / TESTS_NUMBER) << ";"
            << doubleToCSV(((double) decomprTime.count()) / TESTS_NUMBER) << endl;
    }
    csv.close();
}

void lz775Test()
{
    ofstream csv;
    steady_clock::time_point startTime;
    steady_clock::time_point endTime;

    csv.open(csvPath + "lz775.csv");
    for (int fileNum = 1; fileNum <= FILES_NUMBER; ++fileNum)
    {
        cout << "LZ775 " << fileNum << endl;
        string number = ((fileNum >= 10) ? "" : "0") + to_string(fileNum);
        string input = inPath + number;
        string compressed = comprPath + lz775Path + number + ".lz775";
        string decompressed = decomprPath + lz775Path + number + ".unlz775";
        nanoseconds comprTime{};
        nanoseconds decomprTime{};

        csv << fileNum << ";";
        for (int i = 0; i < TESTS_NUMBER; ++i)
        {
            LZ77 lz77(4 * 1024, 1 * 1024);
            startTime = steady_clock::now();
            lz77.encodeFile(input, compressed);
            endTime = steady_clock::now();
            comprTime += duration_cast<nanoseconds>(endTime - startTime);

            LZ77Decoder lz77Decoder;
            startTime = steady_clock::now();
            lz77Decoder.decodeFile(compressed, decompressed);
            endTime = steady_clock::now();
            decomprTime += duration_cast<nanoseconds>(endTime - startTime);

        }
        csv << doubleToCSV(((double) comprTime.count()) / TESTS_NUMBER) << ";"
            << doubleToCSV(((double) decomprTime.count()) / TESTS_NUMBER) << endl;
    }
    csv.close();
}

void lz7710Test()
{
    ofstream csv;
    steady_clock::time_point startTime;
    steady_clock::time_point endTime;

    csv.open(csvPath + "lz7710.csv");
    for (int fileNum = 1; fileNum <= FILES_NUMBER; ++fileNum)
    {
        cout << "LZ7710 " << fileNum << endl;
        string number = ((fileNum >= 10) ? "" : "0") + to_string(fileNum);
        string input = inPath + number;
        string compressed = comprPath + lz7710Path + number + ".lz7710";
        string decompressed = decomprPath + lz7710Path + number + ".unlz7710";
        nanoseconds comprTime{};
        nanoseconds decomprTime{};

        csv << fileNum << ";";
        for (int i = 0; i < TESTS_NUMBER; ++i)
        {
            LZ77 lz77(8 * 1024, 2 * 1024);
            startTime = steady_clock::now();
            lz77.encodeFile(input, compressed);
            endTime = steady_clock::now();
            comprTime += duration_cast<nanoseconds>(endTime - startTime);

            LZ77Decoder lz77Decoder;
            startTime = steady_clock::now();
            lz77Decoder.decodeFile(compressed, decompressed);
            endTime = steady_clock::now();
            decomprTime += duration_cast<nanoseconds>(endTime - startTime);

        }
        csv << doubleToCSV(((double) comprTime.count()) / TESTS_NUMBER) << ";"
            << doubleToCSV(((double) decomprTime.count()) / TESTS_NUMBER) << endl;
    }
    csv.close();
}

void lz7720Test()
{
    ofstream csv;
    steady_clock::time_point startTime;
    steady_clock::time_point endTime;

    csv.open(csvPath + "lz7720.csv");
    for (int fileNum = 1; fileNum <= FILES_NUMBER; ++fileNum)
    {
        cout << "LZ7720 " << fileNum << endl;
        string number = ((fileNum >= 10) ? "" : "0") + to_string(fileNum);
        string input = inPath + number;
        string compressed = comprPath + lz7720Path + number + ".lz7720";
        string decompressed = decomprPath + lz7720Path + number + ".unlz7720";
        nanoseconds comprTime{};
        nanoseconds decomprTime{};

        csv << fileNum << ";";
        for (int i = 0; i < TESTS_NUMBER; ++i)
        {
            LZ77 lz77(16 * 1024, 4 * 1024);
            startTime = steady_clock::now();
            lz77.encodeFile(input, compressed);
            endTime = steady_clock::now();
            comprTime += duration_cast<nanoseconds>(endTime - startTime);

            LZ77Decoder lz77Decoder;
            startTime = steady_clock::now();
            lz77Decoder.decodeFile(compressed, decompressed);
            endTime = steady_clock::now();
            decomprTime += duration_cast<nanoseconds>(endTime - startTime);

        }
        csv << doubleToCSV(((double) comprTime.count()) / TESTS_NUMBER) << ";"
            << doubleToCSV(((double) decomprTime.count()) / TESTS_NUMBER) << endl;
    }
    csv.close();
}

void tocsv()
{
    ifstream fin;
    ofstream fout;
    fin.open(csvPath + "Entropies.csv");
    fout.open(csvPath + "Entorpie.csv");

    char ch;
    while (fin.get(ch))
    {
        if (ch == '.')
            fout << ',';
        else
            fout << ch;
    }
    fin.close();
    fout.close();
}

int main()
{
    calcFrequences();
    calcEntropies();
    shannonFanoTest();
    lz775Test();
    lz7710Test();
    lz7720Test();
    huffmanTest();
    calcCompressionCoeffs();
}