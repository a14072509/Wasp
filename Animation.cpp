#include "Animation.h"
#include "Tokenizer.h"
#include "Channel.h"

bool Animation::Load(const char *filename)
{
    Tokenizer token;
    token.Open(filename);

    token.FindToken("range");
    start = token.GetFloat();
    end = token.GetFloat();

    token.FindToken("numchannels");
    int numChannels = token.GetInt();

    for (int i = 0; i < numChannels; i++)
    {
        token.FindToken("channel");

        Channel *c = new Channel();
        c->load(token);

        channels.push_back(c);
    }
	token.Close();
    return true;
}