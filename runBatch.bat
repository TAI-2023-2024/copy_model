@echo off
cd C:\Users\admin\Desktop\UA\TAI\copy_model

echo KmerSize,Alpha,Threshold,NBits,DefaultNBits,EncodedChars,NonEncodedChars,BitsPerChar,DefaultBitsPerChar,Duration(s) >> Results\results_chry.txt
echo KmerSize,Alpha,Threshold,NBits,DefaultNBits,EncodedChars,NonEncodedChars,BitsPerChar,DefaultBitsPerChar,Duration(s) >> Results\results_biblia.txt
echo KmerSize,Alpha,Threshold,NBits,DefaultNBits,EncodedChars,NonEncodedChars,BitsPerChar,DefaultBitsPerChar,Duration(s) >> Results\results_alice.txt
echo KmerSize,Alpha,Threshold,NBits,DefaultNBits,EncodedChars,NonEncodedChars,BitsPerChar,DefaultBitsPerChar,Duration(s) >> Results\results_lusiadas.txt
echo KmerSize,Alpha,Threshold,NBits,DefaultNBits,EncodedChars,NonEncodedChars,BitsPerChar,DefaultBitsPerChar,Duration(s) >> Results\results_sampledna.txt

(for %%k in (4 6 8 10 12 14 16) do (
    (for %%a in (1 0.1 10 0.01 100) do (
        (for %%t in (0.3 0.5 0.8) do (
            .\cpm.exe -t %%t -a %%a -k %%k -f Data\chry.txt >> Results\results_chry.txt
            .\cpm.exe -t %%t -a %%a -k %%k -f Data\biblia.txt >> Results\results_biblia.txt
            .\cpm.exe -t %%t -a %%a -k %%k -f Data\alice.txt >> Results\results_alice.txt
            .\cpm.exe -t %%t -a %%a -k %%k -f Data\lusiadas.txt >> Results\results_lusiadas.txt
            .\cpm.exe -t %%t -a %%a -k %%k -f Data\sampledna.txt >> Results\results_sampledna.txt
        ))
    ))
))