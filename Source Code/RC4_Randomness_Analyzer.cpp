#include <iostream>
#include <math.h>
#include <fstream>

#define RANDOM_KEY_SIZE 2048
#define RC4_SBOX_SIZE 256
#define MAX_NO_OF_FLIPS 32
#define BYTE_SIZE 8
#define MAX_KEYSTREAM_SIZE 8192
#define MAX_NO_OF_COUNTERS 4096
#define NO_OF_TEST_CASES 1000

using namespace std;

const int KEY_STREAM_LENGTHS_EXPONENT[6] = {2, 4, 8, 32, 128, 1024};

double standardDeviation(int *nums, int size)
{
    int sum = 0;
    for (int i = 0; i < size; i++)
        sum += nums[i];
    double mean = (double)(sum / size);
    double sqdiff = 0;
    for (int i = 0; i < size; i++)
        sqdiff += pow((nums[i] - mean), 2);
    return sqrt(sqdiff / size);
}

int binaryToDecimal(int *b)
{
    int d = 0;
    for (int i = 0; i < 8; ++i)
        d = d * 2 + b[i];
    return d;
}

void generateRandomKey(int *key)
{
    for (int i = 0; i < RANDOM_KEY_SIZE; ++i)
        key[i] = rand() % 2;
}

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void KSA(int *key, int *SBox)
{
    for (int i = 0; i < RC4_SBOX_SIZE; ++i)
        SBox[i] = i;
    for (int i = 0, j = 0; i < RC4_SBOX_SIZE; ++i)
    {
        j = (j + SBox[i] + key[i]) % RC4_SBOX_SIZE;
        swap(&SBox[i], &SBox[j]);
    }
}

void PRGA(int *SBox, int *stream, int noOfBits)
{
    int i = 0, j = 0;
    for (int k = 0; k < noOfBits; ++k)
    {
        i = (i + 1) % RC4_SBOX_SIZE;
        j = (j + SBox[i]) % RC4_SBOX_SIZE;
        swap(&SBox[i], &SBox[j]);
        stream[k] = SBox[SBox[i] + SBox[j] % RC4_SBOX_SIZE] % 2;
    }
}

void FlipBits(int *prevKey, int *modifiedKey, int noOfBits)
{
    for (int i = 0; i < RANDOM_KEY_SIZE; ++i)
        modifiedKey[i] = prevKey[i];
    int b = 0;
    while (noOfBits--)
    {
        modifiedKey[b] = 1 - modifiedKey[rand() % RANDOM_KEY_SIZE];
        b++;
    }
}

int main()
{
    //generating a random key
    int randomKey[RANDOM_KEY_SIZE];
    srand((unsigned int)time(NULL));
    generateRandomKey(randomKey);

    //fetching keyStream from RC4
    int SBox[RC4_SBOX_SIZE], keyStream[MAX_KEYSTREAM_SIZE];
    KSA(randomKey, SBox);
    PRGA(SBox, keyStream, MAX_KEYSTREAM_SIZE);

    //modifing the randomly generated key and fetching the corresponding key stream from RC4
    fstream fout;
    int modifiedKey[RANDOM_KEY_SIZE], modifiedKeyStream[MAX_KEYSTREAM_SIZE], counter[MAX_NO_OF_COUNTERS], streamDifference[MAX_KEYSTREAM_SIZE], binary[BYTE_SIZE], noOfSamples, noOfCounters;
    double randomness[MAX_NO_OF_FLIPS], standardDeviationSum;
    for (int c = 3; c < 13; ++c)
    {
        noOfCounters = pow(2, c);
        fout.open("./randomnessOnUsing" + to_string(noOfCounters) + "Counters.txt");
        for (int l = 0; l < 6; ++l)
        {
            for (int i = 1; i <= MAX_NO_OF_FLIPS; i++)
            {
                standardDeviationSum = 0;
                for (int j = 1; j <= NO_OF_TEST_CASES; j++)
                {
                    //flipping i bits in randomly generated key
                    FlipBits(randomKey, modifiedKey, i);

                    //fetching key stream using modified key
                    KSA(modifiedKey, SBox);
                    PRGA(SBox, modifiedKeyStream, KEY_STREAM_LENGTHS_EXPONENT[l] * BYTE_SIZE);

                    //finding difference in the key
                    for (int k = 0; k < KEY_STREAM_LENGTHS_EXPONENT[l] * BYTE_SIZE; k++)
                        streamDifference[k] = keyStream[k] ^ modifiedKeyStream[k];

                    noOfSamples = KEY_STREAM_LENGTHS_EXPONENT[l] * BYTE_SIZE - BYTE_SIZE + 1;
                    //evaluating counters
                    for (int k = 0; k < noOfCounters; k++)
                        counter[k] = 0;
                    for (int k = 0; k < noOfSamples; k++)
                    {
                        for (int bc = 0; bc < 8; bc++)
                            binary[bc] = streamDifference[k + bc];
                        counter[binaryToDecimal(binary)]++;
                    }
                    standardDeviationSum += standardDeviation(counter, noOfCounters);
                }
                //evaulting randomness using evaluated counters
                randomness[i - 1] = ((standardDeviationSum / NO_OF_TEST_CASES) * noOfCounters) / noOfSamples;
            }
            for (int i = 0; i < 32; i++)
                fout << randomness[i] << " ";
            fout << "\n";
        }
        fout.close();
    }

    return 0;
}