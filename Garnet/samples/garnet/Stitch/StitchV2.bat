Rem Garnet StitchV2_0 -d StitchV2_0.a
Rem Garnet StitchV2_1 -d StitchV2_1.a
Rem Garnet StitchV2_2 -d StitchV2_2.a -g 1153
Rem Garnet StitchV2_3 -d StitchV2_3.a
Rem Garnet StitchV2_4 -d StitchV2_4.a
Rem Garnet StitchV2_5 -d StitchV2_5.a

Garnet StitchV2_0 -d StitchV2_0.a
Garnet StitchV2_1 -d StitchV2_1.a
Garnet StitchV2_2 -d StitchV2_2.a -g 1153
Garnet StitchV2_3 -d StitchV2_3.a
Garnet StitchV2_4 -d StitchV2_4.a
Garnet StitchV2_5 -d StitchV2_5.a

grep "BestFitness" StitchV2_0.a/* > StitchV2_0.a.BestFitness
grep "BestFitness" StitchV2_0.b/* > StitchV2_0.b.BestFitness
grep "BestFitness" StitchV2_1.a/* > StitchV2_1.a.BestFitness
grep "BestFitness" StitchV2_1.b/* > StitchV2_1.b.BestFitness
grep "BestFitness" StitchV2_2.a/* > StitchV2_2.a.BestFitness
grep "BestFitness" StitchV2_2.b/* > StitchV2_2.b.BestFitness
grep "BestFitness" StitchV2_3.a/* > StitchV2_3.a.BestFitness
grep "BestFitness" StitchV2_3.b/* > StitchV2_3.b.BestFitness
grep "BestFitness" StitchV2_4.a/* > StitchV2_4.a.BestFitness
grep "BestFitness" StitchV2_4.b/* > StitchV2_4.b.BestFitness
grep "BestFitness" StitchV2_5.a/* > StitchV2_5.a.BestFitness
grep "BestFitness" StitchV2_5.b/* > StitchV2_5.b.BestFitness

grep "# 0" -A 1 StitchV2_0.a/* | grep -v "# 0" | grep -v -- "--" > StitchV2_0.a.BestIndividual
grep "# 0" -A 1 StitchV2_0.b/* | grep -v "# 0" | grep -v -- "--" > StitchV2_0.b.BestIndividual
grep "# 0" -A 1 StitchV2_1.a/* | grep -v "# 0" | grep -v -- "--" > StitchV2_1.a.BestIndividual
grep "# 0" -A 1 StitchV2_1.b/* | grep -v "# 0" | grep -v -- "--" > StitchV2_1.b.BestIndividual
grep "# 0" -A 1 StitchV2_2.a/* | grep -v "# 0" | grep -v -- "--" > StitchV2_2.a.BestIndividual
grep "# 0" -A 1 StitchV2_2.b/* | grep -v "# 0" | grep -v -- "--" > StitchV2_2.b.BestIndividual
grep "# 0" -A 1 StitchV2_3.a/* | grep -v "# 0" | grep -v -- "--" > StitchV2_3.a.BestIndividual
grep "# 0" -A 1 StitchV2_3.b/* | grep -v "# 0" | grep -v -- "--" > StitchV2_3.b.BestIndividual
grep "# 0" -A 1 StitchV2_4.a/* | grep -v "# 0" | grep -v -- "--" > StitchV2_4.a.BestIndividual
grep "# 0" -A 1 StitchV2_4.b/* | grep -v "# 0" | grep -v -- "--" > StitchV2_4.b.BestIndividual
grep "# 0" -A 1 StitchV2_5.a/* | grep -v "# 0" | grep -v -- "--" > StitchV2_5.a.BestIndividual
grep "# 0" -A 1 StitchV2_5.b/* | grep -v "# 0" | grep -v -- "--" > StitchV2_5.b.BestIndividual

paste StitchV2_0.a.BestFitness StitchV2_0.a.BestIndividual > StitchV2_0.a.tsv
paste StitchV2_0.b.BestFitness StitchV2_0.b.BestIndividual > StitchV2_0.b.tsv
paste StitchV2_1.a.BestFitness StitchV2_1.a.BestIndividual > StitchV2_1.a.tsv
paste StitchV2_1.b.BestFitness StitchV2_1.b.BestIndividual > StitchV2_1.b.tsv
paste StitchV2_2.a.BestFitness StitchV2_2.a.BestIndividual > StitchV2_2.a.tsv
paste StitchV2_2.b.BestFitness StitchV2_2.b.BestIndividual > StitchV2_2.b.tsv
paste StitchV2_3.a.BestFitness StitchV2_3.a.BestIndividual > StitchV2_3.a.tsv
paste StitchV2_3.b.BestFitness StitchV2_3.b.BestIndividual > StitchV2_3.b.tsv
paste StitchV2_4.a.BestFitness StitchV2_4.a.BestIndividual > StitchV2_4.a.tsv
paste StitchV2_4.b.BestFitness StitchV2_4.b.BestIndividual > StitchV2_4.b.tsv
paste StitchV2_5.a.BestFitness StitchV2_5.a.BestIndividual > StitchV2_5.a.tsv
paste StitchV2_5.b.BestFitness StitchV2_5.b.BestIndividual > StitchV2_5.b.tsv
