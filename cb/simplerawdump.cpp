
// Full dump of the blockchain in bitcointools format

// # what should dump all transactions look like??
//for each tx we have
//in\t[txhash]\tcoinbase\t[datetime] OR
//listof
//in\t[txhash]\t[prev_txhash]\t[prevout_n]\t[public_key]\t[datetime]
//then
//for index=0 to number of out in txout
//out\t[txhash]\t[index]\t[publickey]\t[value/1.0e8]\t[datetime]


#include <util.h>
#include <strings.h>
#include <callback.h>

struct SimpleRawDump:public Callback {

    FILE *outFile;
    uint64_t txId;
    bool isCoinBase;
    uint32_t inputId;
    uint32_t outputId;
    uint32_t currBlock;
    uint32_t txVersion;
    uint32_t txIdInBlock;
 //   uint32_t indentLevel;
//    uint8_t spaces[1024];
    optparse::OptionParser parser;
    char datetime_str_buffer [30];
    char transaction_hash_buffer [64];
    //uint8_t *currentTXhash;
    //uint8_t* currentTxHashbuf;
    
    SimpleRawDump() {

        txId = 0;
        currBlock = 0;
       // indentLevel = 0;
      ///  memset(spaces, ' ' , sizeof(spaces));
        //spaces[0] = 0;

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

    
    virtual int init(
        int argc,
        const char *argv[]
    )
    {

        info("dumping the blockchain ...");
        //currentTxHashbuf = (uint8_t*)alloca(2*kSHA256ByteSize + 1);
        outFile = fopen("all-transactions.txt", "w");
        if(!outFile)
            sysErrFatal("couldn't open file txs.txt for writing\n");

        return 0;
    } 
    
    // Called when the second parse of the full chain starts
    virtual void start(
        const Block *s,
        const Block *e
    ) {
        info("second parse started");
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
        
        
        
        auto p = b->chunk->getData();
        SKIP(uint32_t, version, p);
        SKIP(uint256_t, prevBlkHash, p);
        SKIP(uint256_t, blkMerkleRoot, p);
        LOAD(uint32_t, blkTime, p);
        
        time_t blkTime_ = (time_t)blkTime;
        struct tm * dt;
        
        dt = gmtime(&blkTime_);
        strftime(datetime_str_buffer, sizeof(datetime_str_buffer), "%Y-%m-%d-%H-%M-%S", dt);
        
        
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


        #if defined(CLAM)
            auto pBis = p;
            LOAD(uint32_t, nVersion, pBis);
            txVersion = nVersion;
        #endif
        //showHex(hash);
        //currentTXhash = hash;
        //uint8_t* buf = (uint8_t*)alloca(2*kSHA256ByteSize + 1);
        //toHex(currentTxHashbuf, hash, kSHA256ByteSize, true);
        sShowHex(transaction_hash_buffer, hash);
        //fprintf(f, "%s", buf);
        

    }

    // Called when the start of a TX's input array is encountered
    virtual void startInputs(
        const uint8_t *p
    ) {
        inputId = 0;
        isCoinBase = false;
        
    }

    // Called when a TX input is encountered
    virtual void startInput(
        const uint8_t *p
    ) {
        

        static uint256_t gNullHash;
        LOAD(uint256_t, upTXHash, p);
        LOAD(uint32_t, upOutputIndex, p);
        LOAD_VARINT(inputScriptSize, p);

        isCoinBase = (0==memcmp(gNullHash.v, upTXHash.v, sizeof(gNullHash)));
        if(isCoinBase) {
            //uint64_t value = getBaseReward(currBlock);
            fprintf(outFile, "in\t");
            
            fprintf(outFile, "%s\t", transaction_hash_buffer);
            //fShowHex(outFile, currentTXhash); fprintf(outFile, "\t");
            
            fprintf(outFile, "coinbase\t");
            fprintf(outFile, "%s\n", datetime_str_buffer);
            //fprintf(outFile, "%" PRIu64 "", value);
            //fprintf(outFile, "\t%d\n", currBlock - 1);
            //fShowHex(outFile, upTXHash); fprintf(outFile, "\t");
          //  fprintf(outFile, "isCoinBase = true");
            /*fprintf(outFile, 
                "value = %" PRIu64 " # %.08f\n",
                value,
                satoshisToNormaForm(value)
            );
            fprintf(outFile, "%scoinBase = '\n", spaces);
            
                canonicalHexDump(
                    p,
                    inputScriptSize,
                    (const char *)spaces
                );
            
            fprintf(outFile, "%s'\n", spaces);*/
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
    
     uint8_t address[40];
        address[0] = 'X';
        address[1] = 0;

        uint8_t addrType[3];
        uint160_t pubKeyHash;
        int type = solveOutputScript(pubKeyHash.v, outputScript, outputScriptSize, addrType);
        if(likely(0<=type)) hash160ToAddr(address, pubKeyHash.v);

    
    fprintf(outFile, "in\t");
    fShowHex(outFile, downTXHash); fprintf(outFile, "\t");
    fShowHex(outFile, upTXHash); fprintf(outFile, "\t");
    fprintf(outFile, "%" PRIu64 "\t", outputIndex);
    
    
    fprintf(outFile, "%s\t", address);
    //fprintf(outFile, "%" PRIu64 "\t", value); // not in bitcoin-tools output ... why???
    fprintf(outFile, "%s\n", datetime_str_buffer);

    
    }

    // Called when at the end of a TX input
    virtual void endInput(
        const uint8_t *p
    ) {
    }

    // Called when the end of a TX's input array is encountered
    virtual void endInputs(
        const uint8_t *p
    ) {
        
    }

    // Called when the start of a TX's output array is encountered
    virtual void startOutputs(
        const uint8_t *p
    ) {
        outputId = 0;
        
    }

    // Called when a TX output is encountered
    virtual void startOutput(
        const uint8_t *p
    ) {
        
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
    //out\t[txhash]\t[index]\t[publickey]\t[value/1.0e8]\t[datetime]
     uint8_t address[40];
        address[0] = 'X';
        address[1] = 0;

        uint8_t addrType[3];
        uint160_t pubKeyHash;
        int type = solveOutputScript(pubKeyHash.v, outputScript, outputScriptSize, addrType);
        if(likely(0<=type)) hash160ToAddr(address, pubKeyHash.v);
    fprintf(outFile, "out\t");
    fShowHex(outFile, txHash);fprintf(outFile, "\t");
    fprintf(outFile, "%" PRIu64 "\t", outputIndex);
    fprintf(outFile, "%s\t", address);
    fprintf(outFile, "%" PRIu64 "\t", value);
    fprintf(outFile, "%s\n", datetime_str_buffer);
    
    }

    // Called when the end of a TX's output array is encountered
    virtual void endOutputs(
        const uint8_t *p
    ) {
        #if defined(CLAM)
            if(1<txVersion) {
                LOAD_VARINT(strCLAMSpeechLen, p);
                fprintf(outFile, "%stxComment = '\n", spaces);
                    
                        canonicalHexDump(
                            p,
                            strCLAMSpeechLen,
                            (const char *)spaces
                        );
                    
                fprintf(outFile, "%s'\n", spaces);
            }
        #endif

        
        //fprintf(outFile, "%s}\n", spaces);
    }

    // Called when an end of TX is encountered
    virtual void endTX(
        const uint8_t *p
    ) {
        
        ++txIdInBlock;
        ++txId;
    }

    // Called when end list of TX is encountered
    virtual void endTXs(
        const uint8_t *p
    ) {
        
    }

    // Called when an end of block is encountered
    virtual void endBlock(
        const Block *b
    ) {
        
    }

    virtual void      startLC(                                     )       {               }  // Called when longest chain parse starts
    virtual void wrapup()
    {
        fclose(outFile);
        //fclose(inputFile);
        //fclose(blockFile);
        //fclose(txFile);
        info("done\n");
        //exit(0);
    } // Called when the whole chain has been parsed

};

static SimpleRawDump SimpleRawDump;



