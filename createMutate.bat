@echo off
cd C:\Users\admin\Desktop\UA\TAI\copy_model

echo Filename,Probability,Mutations,TrueMutations,FalseMutations >> Results\mutate_chry.txt
echo Filename,Probability,Mutations,TrueMutations,FalseMutations >> Results\mutate_alice.txt

(for %%p in (0.25 0.50 0.75) do (
    .\mutate.exe -p %%p -f Data\chry.txt >> Results\mutate_chry.txt
    .\mutate.exe -p %%p -f Data\alice.txt >> Results\mutate_alice.txt
))