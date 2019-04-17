#include "cachelab.h"
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <strings.h>
#include <math.h>

typedef unsigned long long int address;

struct cacheBit{
	int s;
	int b;
	int E;
	int S;
	int B;
	int hit;
	int miss;
	int evict;
};

struct setLine{
	int recentUsed;
	int valid;
	address tag;
	char * block;
};

struct cacheSet{
	struct setLine * line;
};

struct cache{
	struct cacheSet * set;
};

struct cache makeCache(long long numOfSet, int numOfLine, long long sizeOfBlock)
{
	struct cache nCache;
	struct cacheSet nSet;
	struct setLine nLine;
	int index_set;
	int index_line;

	nCache.set = (struct cacheSet*)malloc(sizeof(struct cacheSet) * numOfSet);

	for(index_set = 0; index_set < numOfSet; index_set++)
	{
		nSet.line = (struct setLine *)malloc(sizeof(struct setLine) * numOfLine);
		nCache.set[index_set] = nSet;

		for(index_line = 0; index_line < numOfLine; index_line++)
		{
			nLine.recentUsed = 0;
			nLine.valid = 0;
			nLine.tag = 0;
			nSet.line[index_line] = nLine;
		}
	}
	return nCache;
};

void deleteCache(struct cache Cache, long long numOfSet, int numOfLine, long long sizeOfBlock)
{
	int index_set;
	for(index_set = 0; index_set < numOfSet; index_set++)
	{
		struct cacheSet nSet = Cache.set[index_set];
		if(nSet.line != NULL)
			free(nSet.line);
	}

	if(Cache.set != NULL)
		free(Cache.set);
}

int emptyLine(struct cacheSet nSet, struct cacheBit nBit)
{
	int numOfLine = nBit.E;
	int index;
	struct setLine nLine;

	for(index = 0; index < numOfLine; index++)
	{
		nLine = nSet.line[index];
		if(nLine.valid == 0)
			return index;
	}
	return -1;
}

int evictLine(struct cacheSet nSet, struct cacheBit nBit, int * oldLine)
{
	int numOfLine = nBit.E;
	int max = nSet.line[0].recentUsed;
	int min = nSet.line[0].recentUsed;
	int i_line;
	int i = 0;

	for(i_line = 1; i_line < numOfLine; i_line++)
	{
		if(min > nSet.line[i_line].recentUsed)
		{
			i = i_line;
			min = nSet.line[i_line].recentUsed;
		}
		if(max < nSet.line[i_line].recentUsed)
			max = nSet.line[i_line].recentUsed;
	}

	oldLine[0] = min;
	oldLine[1] = max;
	return i;
}

struct cacheBit cacheData(struct cache nCache, struct cacheBit nBit, address nAddress)
{
	int index_line;
	int full = 1;

	int numOfLine = nBit.E;
	int pHit = nBit.hit;

	int tagSize = (64 - (nBit.s + nBit.b));
	address inputTag = nAddress >> (nBit.s + nBit.b);
	unsigned long long tmp = nAddress << (tagSize);
	unsigned long long index_set = tmp >> (tagSize + nBit.b);

	struct cacheSet nSet = nCache.set[index_set];

	for(index_line = 0; index_line < numOfLine; index_line++)
	{
		if(nSet.line[index_line].valid)
		{
			if(nSet.line[index_line].tag == inputTag)
			{
				nSet.line[index_line].recentUsed++;
				nBit.hit++;
			}
		}
		else if(!(nSet.line[index_line].valid) && (full))
			full = 0;
	}

	if(pHit == nBit.hit)
		nBit.miss++;
	else
		return nBit;

	int * oldLine = (int*)malloc(sizeof(int) * 2);
	int index_min = evictLine(nSet,nBit,oldLine);

	if(full)
	{
		nBit.evict++;
		nSet.line[index_min].tag = inputTag;
		nSet.line[index_min].recentUsed = oldLine[1]+1;
	}
	else
	{
		int i = emptyLine(nSet,nBit);
		nSet.line[i].tag = inputTag;
		nSet.line[i].valid = 1;
		nSet.line[i].recentUsed = oldLine[1]+1;
	}

	free(oldLine);
	return nBit;
}

int main(int argc, char ** argv)
{
	char opt;
	struct cache nCache;
	struct cacheBit nBit;
	bzero(&nBit,sizeof(nBit));
	long long numOfSet;
	long long sizeOfBlock;
	FILE * open;
	char trace;
	address nAddress;
	int size;
	char * trace_file;

	while((opt = getopt(argc, argv, "s:E:b:t:vh")) != -1)
	{
		switch(opt)
		{
			case 's':
				nBit.s = atoi(optarg);
				break;
			case 'E':
				nBit.E = atoi(optarg);
				break;
			case 'b':
				nBit.b = atoi(optarg);
				break;
			case 't':
				trace_file = optarg;
				break;
			case 'h':
				exit(0);
			default:
				exit(1);
		}	
	}
	numOfSet = pow(2.0, nBit.s);
	sizeOfBlock = pow(2.0, nBit.b);
	nBit.hit = 0;
	nBit.miss = 0;
	nBit.evict = 0;
	nCache = makeCache(numOfSet,nBit.E,sizeOfBlock);

	open = fopen(trace_file, "r");
	if(open != NULL)
	{
		while(fscanf(open," %c %llx,%d", &trace, &nAddress, &size) == 3)
		{
			switch(trace)
			{
				case 'I':
					break;
				case 'L':
					nBit = cacheData(nCache,nBit,nAddress);
					break;
				case 'S':
					nBit = cacheData(nCache,nBit,nAddress);
					break;
				case 'M':
					nBit = cacheData(nCache,nBit,nAddress);
					nBit = cacheData(nCache,nBit,nAddress);
					break;
				default:
					break;
			}
		}
	}

	printSummary(nBit.hit,nBit.miss,nBit.evict);
	deleteCache(nCache,numOfSet,nBit.E,sizeOfBlock);
	fclose(open);
	return 0;
}
