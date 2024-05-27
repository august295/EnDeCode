#include "rc4.h"

int rc4_init(Rc4Context* context, const uint8_t* key, size_t length)
{
    uint16_t i;
    uint16_t j;
    uint8_t  temp;

    // Check parameters
    if (context == NULL || key == NULL)
        return -1;

    // Clear context
    context->i = 0;
    context->j = 0;

    // Initialize the S array with identity permutation
    for (i = 0; i < 256; i++)
    {
        context->s[i] = i;
    }

    // S is then processed for 256 iterations
    for (i = 0, j = 0; i < 256; i++)
    {
        // Randomize the permutations using the supplied key
        j = (j + context->s[i] + key[i % length]) % 256;

        // Swap the values of S[i] and S[j]
        temp          = context->s[i];
        context->s[i] = context->s[j];
        context->s[j] = temp;
    }

    // RC4 context successfully initialized
    return 0;
}

void rc4_crypt(Rc4Context* context, const uint8_t* input, uint8_t* output, size_t length)
{
    uint8_t temp;

    // Restore context
    uint16_t i = context->i;
    uint16_t j = context->j;
    uint8_t* s = context->s;

    // Encryption loop
    while (length > 0)
    {
        // Adjust indices
        i = (i + 1) % 256;
        j = (j + s[i]) % 256;

        // Swap the values of S[i] and S[j]
        temp = s[i];
        s[i] = s[j];
        s[j] = temp;

        // Valid input and output?
        if (input != NULL && output != NULL)
        {
            // XOR the input data with the RC4 stream
            *output = *input ^ s[(s[i] + s[j]) % 256];

            // Increment data pointers
            input++;
            output++;
        }

        // Remaining bytes to process
        length--;
    }

    // Save context
    context->i = i;
    context->j = j;
}