# Bloom Filter variants in C++
## This repository is dedicated to recreate recognized Bloom Filter variants in C++ with evaluations provided by a Bus ticket checking system.
Current supporting variants:
1. Standard Bloom Filter
2. Counting Bloom Filter
3. Scalable Bloom Filter
4. Dynamic Bloom Filter
5. Deletable Bloom Filter
6. One Hashing Bloom Filter
7. Variable-Increment Bloom Filter (underdevelopment)

- Aim: robust evaluation environment/ framework for testing and comparing Bloom filter variants
-- result: basic implementation. Still require direct interaction with the native code for adding new variants, hashing tecniques
-- what is done:
= an original and customizable bit manipulations implementation that can be further optimized for Bloom filters usage
= the foundation design of an abstraction for all Bloom filter variants 
-- limitation and solution: might be not friendly for new users to implement some of the new non-classic bf variants or apply different hashing techniques other than the supporting ones -> add further abstraction to the filters' base structure, remake the hashing techniques components as an completely injectable dependency; missing an interface to interact with components from other system under any form -> a layer of abstraction for compatibility with non-native components (such as database, POS device interface, network sockets...) through APIs, JDBC driver,... etc; missing meaningful charts and graphs other than a table and raw data as logs -> extra internal implementation to process data into more ingestable forms for graphs and charts

- Aim: curated databases for evaluations
-- result: use-case specific databases; moderate size data
-- what is done:
= data is row-uniquely created and shuffle every run
= data is use-case specfically designed for practical usage and adaption
= data generation script is included
-- limitation and solution: should have a generic generator that support creating data with multiple parameters such as length in bytes/bits, non-static length,... -> an advance data generating interface that allow user to inject their own data or rules in their desired output data 

- Aim: a table of generic measurements
-- result: generic, yet impractical metrics; use of hash functions and hashing techniques are considered; new generations of BF variants, the hybrid BFs are brought into considerations
-- what is done:
= uniformed measurements strategy and conditions on the variations
= despite simple metrics, variations various novel modifications and settings are considered in evaluating
-- limitation and solution: simple metrics, missing practical evaluating strategies and evaluations for other functionalities of the filters such as deletions -> design more advance evaluation strategies with corresponding metrics; missing the estimation of the overhead to justify the false positives caused by the filters -> research for different recovery strategies native to bloom filter applications, then implement bf applications into real-world system then monitor and make comparisons on those implementations for error cases
