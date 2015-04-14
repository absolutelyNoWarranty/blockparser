
// Full dump of the blockchain

#include <util.h>
#include <strings.h>
#include <callback.h>

struct SimpleRawDump:public Callback {

    FILE *txFile;
    uint64_t txId;
    bool isCoinBase;
    uint32_t inputId;
    uint32_t outputId;
    uint32_t currBlock;
    uint32_t txVersion;
    uint32_t txIdInBlock;
    uint32_t indentLevel;
    uint8_t spaces[1024];
    optparse::OptionParser parser;

    //std::unordered_map<const uint8_t, int> hash_to_id;
    
    SimpleRawDump() {

        txId = 0;
        currBlock = 0;
        indentLevel = 0;
        memset(spaces, ' ' , sizeof(spaces));
        spaces[0] = 0;

        parser
            .usage("")
            .version("")
            .description("full dump of the block chain")
            .epilog("")
        ;
    }

    virtual const char                   *name() const         { return "SimpleRawDump";  }
    virtual const optparse::OptionParser *optionParser() const { return &parser;    }
    virtual bool                         needTXHash() const    { return true;       }

    
    /*virtual int init(
        int argc,
        const char *argv[]
    )
    {

        info("dumping the blockchain ...");

        //txFile = fopen("transactions.txt", "w");
        //if(!txFile) sysErrFatal("couldn't open file txs.txt for writing\n");


        return 0;
    } */   
    
    // Called when the second parse of the full chain starts
    virtual void start(
        const Block *s,
        const Block *e
    ) {
        printf("T1\tT2\tValue\tIn_Block\n");
    }

    // Called when a new block is encountered
    virtual void startBlock(
        const Block *b,
        uint64_t    chainSize
    ) {
        // cutoff early
        //if(190 < b->height) wrapup();
        
        currBlock = b->height;
        if ((currBlock%10000) == 0)
            info("On block %d", currBlock);
        
        /*
        txIdInBlock = 0;
        currBlock = b->height;

        printf(
            "block %d",
            (int)(-1+b->height)
        );
        printf("\t");
     
        showHex(b->hash);*/
    }

    // Called when start list of TX is encountered
    virtual void startTXs(
        const uint8_t *p
    ) {

    }

    // Called when a new TX is encountered
    virtual void startTX(
        const uint8_t *p,
        const uint8_t *hash
    ) {
/*
        printf(
            "%stx%d = {\n",
            spaces,
            txIdInBlock
        );
        */

        #if defined(CLAM)
            auto pBis = p;
            LOAD(uint32_t, nVersion, pBis);
            txVersion = nVersion;
        #endif
/*
        printf("%stxHash = '", spaces);*/
        showHex(hash);printf("\t");
        /*printf("'\n");*/
    }

    // Called when the start of a TX's input array is encountered
    virtual void startInputs(
        const uint8_t *p
    ) {
        inputId = 0;
        isCoinBase = false;
       // printf("%sinputs = {\n", spaces);
        
    }

    // Called when a TX input is encountered
    virtual void startInput(
        const uint8_t *p
    ) {
    /*
        printf(
            "%sinput%d = {\n",
            spaces,
            (int)inputId
        );*/
        

        static uint256_t gNullHash;
        LOAD(uint256_t, upTXHash, p);
        LOAD(uint32_t, upOutputIndex, p);
        LOAD_VARINT(inputScriptSize, p);

    /*    printf("%sscript = '\n", spaces);
            
                showScript(p, inputScriptSize, 0, (const char *)spaces);
            
        printf("%s'\n", spaces);*/

        isCoinBase = (0==memcmp(gNullHash.v, upTXHash.v, sizeof(gNullHash)));
        if(isCoinBase) {
            uint64_t value = getBaseReward(currBlock);
            printf("COINBASE\t");
            printf("%" PRIu64 "", value);
            printf("\t%d\n", currBlock - 1);
            //showHex(upTXHash); printf("\t");
          //  printf("isCoinBase = true");
            /*printf(
                "value = %" PRIu64 " # %.08f\n",
                value,
                satoshisToNormaForm(value)
            );
            printf("%scoinBase = '\n", spaces);
            
                canonicalHexDump(
                    p,
                    inputScriptSize,
                    (const char *)spaces
                );
            
            printf("%s'\n", spaces);*/
        }
    }

    // Called exactly like startInput, but with a much richer context
    virtual void edge(
        uint64_t      value,                // Number of satoshis coming in on this input from upstream transaction
        const uint8_t *upTXHash,            // sha256 of upstream transaction
        uint64_t      outputIndex,          // Index of output in upstream transaction
        const uint8_t *outputScript,        // Raw script (challenge to spender) carried by output in upstream transaction
        uint64_t      outputScriptSize,     // Byte size of script carried by output in upstream transaction
        const uint8_t *downTXHash,          // sha256 of current (downstream) transaction
        uint64_t      inputIndex,           // Index of input in downstream transaction
        const uint8_t *inputScript,         // Raw script (answer to challenge) carried by input in downstream transaction
        uint64_t      inputScriptSize       // Byte size of script carried by input in downstream transaction
    )
    {
    showHex(upTXHash); printf("\t"); printf("%" PRIu64 "", value);
    printf("\t%d\n", currBlock - 1);
     /*   printf(
            "%svalue = %" PRIu64 " # %.08f\n",
            spaces,
            value,
            satoshisToNormaForm(value)
        );

        printf(
            "%ssourceTXOutputIndex = %d\n",
            spaces,
            (int)outputIndex
        );

        printf(
            "%ssourceTXHash = '",
            spaces
        );
        showHex(upTXHash);
        printf("'\n");*/
    }

    // Called when at the end of a TX input
    virtual void endInput(
        const uint8_t *p
    ) {
       /* if(!isCoinBase) {
            printf("%sisCoinBase = false\n", spaces);
        }

        
        printf("%s}\n", spaces);
        ++inputId;*/
    }

    // Called when the end of a TX's input array is encountered
    virtual void endInputs(
        const uint8_t *p
    ) {
        
        //printf("%s}\n", spaces);
    }

    // Called when the start of a TX's output array is encountered
    virtual void startOutputs(
        const uint8_t *p
    ) {
        outputId = 0;
       // printf("%soutputs = {\n", spaces);
        
    }

    // Called when a TX output is encountered
    virtual void startOutput(
        const uint8_t *p
    ) {
       /* printf(
            "%soutput%d = {\n",
            spaces,
            (int)outputId
        );*/
        
    }

    // Called when an output has been fully parsed
    virtual void endOutput(
        const uint8_t *p,                   // Pointer to TX output raw data
        uint64_t      value,                // Number of satoshis on this output
        const uint8_t *txHash,              // sha256 of the current transaction
        uint64_t      outputIndex,          // Index of this output in the current transaction
        const uint8_t *outputScript,        // Raw script (challenge to would-be spender) carried by this output
        uint64_t      outputScriptSize      // Byte size of raw script
    )
    {
   /*     printf(
            "%svalue = %" PRIu64 " # %.08f\n",
            spaces,
            value,
            satoshisToNormaForm(value)
        );

        printf("%sscript = '\n", spaces);
        showScript(outputScript, outputScriptSize, 0, (const char *)spaces);
        printf("%s'\n", spaces);

        showScriptInfo(outputScript, outputScriptSize, spaces);

        
        printf("%s}\n", spaces);
        ++outputId;*/
    }

    // Called when the end of a TX's output array is encountered
    virtual void endOutputs(
        const uint8_t *p
    ) {
        #if defined(CLAM)
            if(1<txVersion) {
                LOAD_VARINT(strCLAMSpeechLen, p);
                printf("%stxComment = '\n", spaces);
                    
                        canonicalHexDump(
                            p,
                            strCLAMSpeechLen,
                            (const char *)spaces
                        );
                    
                printf("%s'\n", spaces);
            }
        #endif

        
        //printf("%s}\n", spaces);
    }

    // Called when an end of TX is encountered
    virtual void endTX(
        const uint8_t *p
    ) {
        
       // printf("%s}\n", spaces);
        ++txIdInBlock;
        ++txId;
    }

    // Called when end list of TX is encountered
    virtual void endTXs(
        const uint8_t *p
    ) {
        
      //  printf("%s}\n", spaces);
    }

    // Called when an end of block is encountered
    virtual void endBlock(
        const Block *b
    ) {
        
      //  printf("%s}\n", spaces);
    }

    virtual void      startLC(                                     )       {               }  // Called when longest chain parse starts
    virtual void wrapup()
    {
        //fclose(outputFile);
        //fclose(inputFile);
        //fclose(blockFile);
        //fclose(txFile);
        info("done\n");
        //exit(0);
    } // Called when the whole chain has been parsed

};

static SimpleRawDump SimpleRawDump;

