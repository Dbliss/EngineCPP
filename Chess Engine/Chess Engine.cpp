#include <iostream>
#include "engine.h"
#include "engine2.h"
#include <chrono>
#include <string>
#include <SFML/Graphics.hpp>
#include "BoardDisplay.h"
#include "chess.h"
#include <thread>
#include <deque>
#include <random>
#include "zobrist.h"
#include <tuple>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <cstdint>

const char* fenArrayShort[] = {
    "r1bqk1r1/1p1p1n2/p1n2pN1/2p1b2Q/2P1Pp2/1PN5/PB4PP/R4RK1 w q - - bm Rxf4",
    "r1n2N1k/2n2K1p/3pp3/5Pp1/b5R1/8/1PPP4/8 w - - bm Ng6",
    "r1b1r1k1/1pqn1pbp/p2pp1p1/P7/1n1NPP1Q/2NBBR2/1PP3PP/R6K w - - bm f5",
    "5b2/p2k1p2/P3pP1p/n2pP1p1/1p1P2P1/1P1KBN2/7P/8 w - - bm Nxg5",
    "r3kbnr/1b3ppp/pqn5/1pp1P3/3p4/1BN2N2/PP2QPPP/R1BR2K1 w kq - - bm Bxf7",
    "r2r2k1/1p1n1pp1/4pnp1/8/PpBRqP2/1Q2B1P1/1P5P/R5K1 b - - bm Nc5",
    "2rq1rk1/pb1n1ppN/4p3/1pb5/3P1Pn1/P1N5/1PQ1B1PP/R1B2RK1 b - - bm Nde5",
    "r2qk2r/ppp1bppp/2n5/3p1b2/3P1Bn1/1QN1P3/PP3P1P/R3KBNR w KQkq - bm Qxd5",
    "rnb1kb1r/p4p2/1qp1pn2/1p2N2p/2p1P1p1/2N3B1/PPQ1BPPP/3RK2R w Kkq - bm Ng6",
    "5rk1/pp1b4/4pqp1/2Ppb2p/1P2p3/4Q2P/P3BPP1/1R3R1K b - - bm d4",
    "r1b2r1k/ppp2ppp/8/4p3/2BPQ3/P3P1K1/1B3PPP/n3q1NR w - - bm dxe5, Nf3",
    "1nkr1b1r/5p2/1q2p2p/1ppbP1p1/2pP4/2N3B1/1P1QBPPP/R4RK1 w - - bm Nxd5",
    "1nrq1rk1/p4pp1/bp2pn1p/3p4/2PP1B2/P1PB2N1/4QPPP/1R2R1K1 w - - bm Qd2, Bc2",
    "5k2/1rn2p2/3pb1p1/7p/p3PP2/PnNBK2P/3N2P1/1R6 w - - bm Nf3",
    "8/p2p4/r7/1k6/8/pK5Q/P7/b7 w - - bm Qd3",
};

const char* fenArrayMedium[] = {
"r1bqk1r1/1p1p1n2/p1n2pN1/2p1b2Q/2P1Pp2/1PN5/PB4PP/R4RK1 w q - - bm Rxf4",
"r1n2N1k/2n2K1p/3pp3/5Pp1/b5R1/8/1PPP4/8 w - - bm Ng6",
"r1b1r1k1/1pqn1pbp/p2pp1p1/P7/1n1NPP1Q/2NBBR2/1PP3PP/R6K w - - bm f5",
"5b2/p2k1p2/P3pP1p/n2pP1p1/1p1P2P1/1P1KBN2/7P/8 w - - bm Nxg5",
"r3kbnr/1b3ppp/pqn5/1pp1P3/3p4/1BN2N2/PP2QPPP/R1BR2K1 w kq - - bm Bxf7",
"r2r2k1/1p1n1pp1/4pnp1/8/PpBRqP2/1Q2B1P1/1P5P/R5K1 b - - bm Nc5",
"8/3Pk1p1/1p2P1K1/1P1Bb3/7p/7P/6P1/8 w - - bm g4",
"4rrk1/Rpp3pp/6q1/2PPn3/4p3/2N5/1P2QPPP/5RK1 w - - am Rxb7",
"2q2rk1/2p2pb1/PpP1p1pp/2n5/5B1P/3Q2P1/4PPN1/2R3K1 w - - bm Rxc5",
"rnbq1r1k/4p1bP/p3p3/1pn5/8/2Np1N2/PPQ2PP1/R1B1KB1R w KQ - bm Nh4",
"4b1k1/1p3p2/4pPp1/p2pP1P1/P2P4/1P1B4/8/2K5 w - - bm b4",
"8/7p/5P1k/1p5P/5p2/2p1p3/P1P1P1P1/1K3Nb1 w - - bm Ng3",
"r3kb1r/ppnq2pp/2n5/4pp2/1P1PN3/P4N2/4QPPP/R1B1K2R w KQkq - bm Nxe5",
"b4r1k/6bp/3q1ppN/1p2p3/3nP1Q1/3BB2P/1P3PP1/2R3K1 w - - bm Rc8",
"r3k2r/5ppp/3pbb2/qp1Np3/2BnP3/N7/PP1Q1PPP/R3K2R w KQkq - bm Nxb5",
"r1k1n2n/8/pP6/5R2/8/1b1B4/4N3/1K5N w - - bm b7",
"1k6/bPN2pp1/Pp2p3/p1p5/2pn4/3P4/PPR5/1K6 w - - bm Na8",
"8/6N1/3kNKp1/3p4/4P3/p7/P6b/8 w - - bm exd5",
"r1b1k2r/pp3ppp/1qn1p3/2bn4/8/6P1/PPN1PPBP/RNBQ1RK1 w kq - bm a3",
"r3kb1r/3n1ppp/p3p3/1p1pP2P/P3PBP1/4P3/1q2B3/R2Q1K1R b kq - bm Bc5",
"3q1rk1/2nbppb1/pr1p1n1p/2pP1Pp1/2P1P2Q/2N2N2/1P2B1PP/R1B2RK1 w - - bm Nxg5",
"8/2k5/N3p1p1/2KpP1P1/b2P4/8/8/8 b - - bm Kb7",
"2r1rbk1/1pqb1p1p/p2p1np1/P4p2/3NP1P1/2NP1R1Q/1P5P/R5BK w - - bm Nxf5",
"rnb2rk1/pp2q2p/3p4/2pP2p1/2P1Pp2/2N5/PP1QBRPP/R5K1 w - - bm h4",
"5rk1/p1p1rpb1/q1Pp2p1/3Pp2p/4Pn2/1R4N1/PPPQ1PPP/R5K1 w - - bm Rb4",
"8/4nk2/1p3p2/1r1p2pp/1P1R1N1P/6P1/3KPP2/8 w - - bm Nd3",
"4kbr1/1b1nqp2/2p1p3/2N4p/1p1PP1pP/1PpQ2B1/4BPP1/r4RK1 w - - bm Nxb7",
"r1b2rk1/p2nqppp/1ppbpn2/3p4/2P5/1PN1PN2/PBQPBPPP/R4RK1 w - - bm cxd5",
"r1b1kq1r/1p1n2bp/p2p2p1/3PppB1/Q1P1N3/8/PP2BPPP/R4RK1 w kq - bm f4",
"r4r1k/p1p3bp/2pp2p1/4nb2/N1P4q/1P5P/PBNQ1PP1/R4RK1 b - - bm Nf3",
"6k1/pb1r1qbp/3p1p2/2p2p2/2P1rN2/1P1R3P/PB3QP1/3R2K1 b - - bm Bh6",
"2r2r2/1p1qbkpp/p2ppn2/P1n1p3/4P3/2N1BB2/QPP2PPP/R4RK1 w - - bm b4",
"r1bq1rk1/p4ppp/3p2n1/1PpPp2n/4P2P/P1PB1PP1/2Q1N3/R1B1K2R b KQ - bm c4",
"2b1r3/5pkp/6p1/4P3/QppqPP2/5RPP/6BK/8 b - - bm c3",
"r2q1rk1/1p2bpp1/p1b2n1p/8/5B2/2NB4/PP1Q1PPP/3R1RK1 w - - bm Bxh6",
"r2qr1k1/pp2bpp1/2pp3p/4nbN1/2P4P/4BP2/PPPQ2P1/1K1R1B1R w - - bm Be2",
"r2qr1k1/pp1bbp2/n5p1/2pPp2p/8/P2PP1PP/1P2N1BK/R1BQ1R2 w - - bm d6",
"8/8/R7/1b4k1/5p2/1B3r2/7P/7K w - - bm h4",
"rq6/5k2/p3pP1p/3p2p1/6PP/1PB1Q3/2P5/1K6 w - - bm Qd3",
"q2B2k1/pb4bp/4p1p1/2p1N3/2PnpP2/PP3B2/6PP/2RQ2K1 b - - bm Qxd8",
"4rrk1/pp4pp/3p4/3P3b/2PpPp1q/1Q5P/PB4B1/R4RK1 b - - bm Rf6",
"rr1nb1k1/2q1b1pp/pn1p1p2/1p1PpNPP/4P3/1PP1BN2/2B2P2/R2QR1K1 w - - bm g6",
"r3k2r/4qn2/p1p1b2p/6pB/P1p5/2P5/5PPP/RQ2R1K1 b kq - bm Kf8",
"8/1pp5/p3k1pp/8/P1p2PPP/2P2K2/1P3R2/5r2 b - - am Rxf2",
"1r3rk1/2qbppbp/3p1np1/nP1P2B1/2p2P2/2N1P2P/1P1NB1P1/R2Q1RK1 b - - bm Qb6",
"8/2pN1k2/p4p1p/Pn1R4/3b4/6Pp/1P3K1P/8 w - - bm Ke1",
"5r1k/1p4bp/3p1q2/1NpP1b2/1pP2p2/1Q5P/1P1KBP2/r2RN2R b - - bm f3",
"r3kb1r/pbq2ppp/1pn1p3/2p1P3/1nP5/1P3NP1/PB1N1PBP/R2Q1RK1 w kq - bm a3",
"5rk1/n2qbpp1/pp2p1p1/3pP1P1/PP1P3P/2rNPN2/R7/1Q3RK1 w - - bm h5",
"r5k1/1bqp1rpp/2n1p3/1p4p1/1b2PP2/2NBB1P1/PPPQ4/2KR3R w - - bm a3",
"1r4k1/1nq3pp/pp1pp1r1/8/PPP2P2/6P1/5N1P/2RQR1K1 w - - bm f5",
"q5k1/p2p2bp/1p1p2r1/2p1np2/6p1/1PP2PP1/P2PQ1KP/4R1NR b - - bm Qd5",
"r4rk1/ppp2ppp/1nnb4/8/1P1P3q/PBN1B2P/4bPP1/R2QR1K1 w - - bm Qxe2",
"1r3k2/2N2pp1/1pR2n1p/4p3/8/1P1K1P2/P5PP/8 w - - bm Kc4",
"6r1/6r1/2p1k1pp/p1pbP2q/Pp1p1PpP/1P1P2NR/1KPQ3R/8 b - - bm Qf5",
"r1b1kb1r/1p1npppp/p2p1n2/6B1/3NPP2/q1N5/P1PQ2PP/1R2KB1R w Kkq - bm Bxf6",
"r3r1k1/1bq2ppp/p1p2n2/3ppPP1/4P3/1PbB4/PBP1Q2P/R4R1K w - - bm gxf6",
"r4rk1/ppq3pp/2p1Pn2/4p1Q1/8/2N5/PP4PP/2KR1R2 w - - bm Rxf6",
"r1bqr1k1/3n1ppp/p2p1b2/3N1PP1/1p1B1P2/1P6/1PP1Q2P/2KR2R1 w - - bm Qxe8",
"5rk1/1ppbq1pp/3p3r/pP1PppbB/2P5/P1BP4/5PPP/3QRRK1 b - - bm Bc1" };

const char* fenArray[] = {
"r1bqk1r1/1p1p1n2/p1n2pN1/2p1b2Q/2P1Pp2/1PN5/PB4PP/R4RK1 w q - - bm Rxf4",
"r1n2N1k/2n2K1p/3pp3/5Pp1/b5R1/8/1PPP4/8 w - - bm Ng6",
"r1b1r1k1/1pqn1pbp/p2pp1p1/P7/1n1NPP1Q/2NBBR2/1PP3PP/R6K w - - bm f5",
"5b2/p2k1p2/P3pP1p/n2pP1p1/1p1P2P1/1P1KBN2/7P/8 w - - bm Nxg5",
"r3kbnr/1b3ppp/pqn5/1pp1P3/3p4/1BN2N2/PP2QPPP/R1BR2K1 w kq - - bm Bxf7",
"r2r2k1/1p1n1pp1/4pnp1/8/PpBRqP2/1Q2B1P1/1P5P/R5K1 b - - bm Nc5",
"2rq1rk1/pb1n1ppN/4p3/1pb5/3P1Pn1/P1N5/1PQ1B1PP/R1B2RK1 b - - bm Nde5",
"r2qk2r/ppp1bppp/2n5/3p1b2/3P1Bn1/1QN1P3/PP3P1P/R3KBNR w KQkq - bm Qxd5",
"rnb1kb1r/p4p2/1qp1pn2/1p2N2p/2p1P1p1/2N3B1/PPQ1BPPP/3RK2R w Kkq - bm Ng6",
"5rk1/pp1b4/4pqp1/2Ppb2p/1P2p3/4Q2P/P3BPP1/1R3R1K b - - bm d4",
"r1b2r1k/ppp2ppp/8/4p3/2BPQ3/P3P1K1/1B3PPP/n3q1NR w - - bm dxe5, Nf3",
"1nkr1b1r/5p2/1q2p2p/1ppbP1p1/2pP4/2N3B1/1P1QBPPP/R4RK1 w - - bm Nxd5",
"1nrq1rk1/p4pp1/bp2pn1p/3p4/2PP1B2/P1PB2N1/4QPPP/1R2R1K1 w - - bm Qd2, Bc2",
"5k2/1rn2p2/3pb1p1/7p/p3PP2/PnNBK2P/3N2P1/1R6 w - - bm Nf3",
"8/p2p4/r7/1k6/8/pK5Q/P7/b7 w - - bm Qd3",
"1b1rr1k1/pp1q1pp1/8/NP1p1b1p/1B1Pp1n1/PQR1P1P1/4BP1P/5RK1 w - - bm Nc6",
"1r3rk1/6p1/p1pb1qPp/3p4/4nPR1/2N4Q/PPP4P/2K1BR2 b - - bm Rxb2",
"r1b1kb1r/1p1n1p2/p3pP1p/q7/3N3p/2N5/P1PQB1PP/1R3R1K b kq - bm Qg5",
"3kB3/5K2/7p/3p4/3pn3/4NN2/8/1b4B1 w - - bm Nf5",
"1nrrb1k1/1qn1bppp/pp2p3/3pP3/N2P3P/1P1B1NP1/PBR1QPK1/2R5 w - - bm Bxh7",
"3rr1k1/1pq2b1p/2pp2p1/4bp2/pPPN4/4P1PP/P1QR1PB1/1R4K1 b - - bm Rc8",
"r4rk1/p2nbpp1/2p2np1/q7/Np1PPB2/8/PPQ1N1PP/1K1R3R w - - bm h4",
"r3r2k/1bq1nppp/p2b4/1pn1p2P/2p1P1QN/2P1N1P1/PPBB1P1R/2KR4 w - - bm Ng6",
"r2q1r1k/3bppbp/pp1p4/2pPn1Bp/P1P1P2P/2N2P2/1P1Q2P1/R3KB1R w KQ - am b3",
"2kb4/p7/r1p3p1/p1P2pBp/R2P3P/2K3P1/5P2/8 w - - bm Bxd8",
"rqn2rk1/pp2b2p/2n2pp1/1N2p3/5P1N/1PP1B3/4Q1PP/R4RK1 w - - bm Nxg6",
"8/3Pk1p1/1p2P1K1/1P1Bb3/7p/7P/6P1/8 w - - bm g4",
"4rrk1/Rpp3pp/6q1/2PPn3/4p3/2N5/1P2QPPP/5RK1 w - - am Rxb7",
"2q2rk1/2p2pb1/PpP1p1pp/2n5/5B1P/3Q2P1/4PPN1/2R3K1 w - - bm Rxc5",
"rnbq1r1k/4p1bP/p3p3/1pn5/8/2Np1N2/PPQ2PP1/R1B1KB1R w KQ - bm Nh4",
"4b1k1/1p3p2/4pPp1/p2pP1P1/P2P4/1P1B4/8/2K5 w - - bm b4",
"8/7p/5P1k/1p5P/5p2/2p1p3/P1P1P1P1/1K3Nb1 w - - bm Ng3",
"r3kb1r/ppnq2pp/2n5/4pp2/1P1PN3/P4N2/4QPPP/R1B1K2R w KQkq - bm Nxe5",
"b4r1k/6bp/3q1ppN/1p2p3/3nP1Q1/3BB2P/1P3PP1/2R3K1 w - - bm Rc8",
"r3k2r/5ppp/3pbb2/qp1Np3/2BnP3/N7/PP1Q1PPP/R3K2R w KQkq - bm Nxb5",
"r1k1n2n/8/pP6/5R2/8/1b1B4/4N3/1K5N w - - bm b7",
"1k6/bPN2pp1/Pp2p3/p1p5/2pn4/3P4/PPR5/1K6 w - - bm Na8",
"8/6N1/3kNKp1/3p4/4P3/p7/P6b/8 w - - bm exd5",
"r1b1k2r/pp3ppp/1qn1p3/2bn4/8/6P1/PPN1PPBP/RNBQ1RK1 w kq - bm a3",
"r3kb1r/3n1ppp/p3p3/1p1pP2P/P3PBP1/4P3/1q2B3/R2Q1K1R b kq - bm Bc5",
"3q1rk1/2nbppb1/pr1p1n1p/2pP1Pp1/2P1P2Q/2N2N2/1P2B1PP/R1B2RK1 w - - bm Nxg5",
"8/2k5/N3p1p1/2KpP1P1/b2P4/8/8/8 b - - bm Kb7",
"2r1rbk1/1pqb1p1p/p2p1np1/P4p2/3NP1P1/2NP1R1Q/1P5P/R5BK w - - bm Nxf5",
"rnb2rk1/pp2q2p/3p4/2pP2p1/2P1Pp2/2N5/PP1QBRPP/R5K1 w - - bm h4",
"5rk1/p1p1rpb1/q1Pp2p1/3Pp2p/4Pn2/1R4N1/PPPQ1PPP/R5K1 w - - bm Rb4",
"8/4nk2/1p3p2/1r1p2pp/1P1R1N1P/6P1/3KPP2/8 w - - bm Nd3",
"4kbr1/1b1nqp2/2p1p3/2N4p/1p1PP1pP/1PpQ2B1/4BPP1/r4RK1 w - - bm Nxb7",
"r1b2rk1/p2nqppp/1ppbpn2/3p4/2P5/1PN1PN2/PBQPBPPP/R4RK1 w - - bm cxd5",
"r1b1kq1r/1p1n2bp/p2p2p1/3PppB1/Q1P1N3/8/PP2BPPP/R4RK1 w kq - bm f4",
"r4r1k/p1p3bp/2pp2p1/4nb2/N1P4q/1P5P/PBNQ1PP1/R4RK1 b - - bm Nf3",
"6k1/pb1r1qbp/3p1p2/2p2p2/2P1rN2/1P1R3P/PB3QP1/3R2K1 b - - bm Bh6",
"2r2r2/1p1qbkpp/p2ppn2/P1n1p3/4P3/2N1BB2/QPP2PPP/R4RK1 w - - bm b4",
"r1bq1rk1/p4ppp/3p2n1/1PpPp2n/4P2P/P1PB1PP1/2Q1N3/R1B1K2R b KQ - bm c4",
"2b1r3/5pkp/6p1/4P3/QppqPP2/5RPP/6BK/8 b - - bm c3",
"r2q1rk1/1p2bpp1/p1b2n1p/8/5B2/2NB4/PP1Q1PPP/3R1RK1 w - - bm Bxh6",
"r2qr1k1/pp2bpp1/2pp3p/4nbN1/2P4P/4BP2/PPPQ2P1/1K1R1B1R w - - bm Be2",
"r2qr1k1/pp1bbp2/n5p1/2pPp2p/8/P2PP1PP/1P2N1BK/R1BQ1R2 w - - bm d6",
"8/8/R7/1b4k1/5p2/1B3r2/7P/7K w - - bm h4",
"rq6/5k2/p3pP1p/3p2p1/6PP/1PB1Q3/2P5/1K6 w - - bm Qd3",
"q2B2k1/pb4bp/4p1p1/2p1N3/2PnpP2/PP3B2/6PP/2RQ2K1 b - - bm Qxd8",
"4rrk1/pp4pp/3p4/3P3b/2PpPp1q/1Q5P/PB4B1/R4RK1 b - - bm Rf6",
"rr1nb1k1/2q1b1pp/pn1p1p2/1p1PpNPP/4P3/1PP1BN2/2B2P2/R2QR1K1 w - - bm g6",
"r3k2r/4qn2/p1p1b2p/6pB/P1p5/2P5/5PPP/RQ2R1K1 b kq - bm Kf8",
"8/1pp5/p3k1pp/8/P1p2PPP/2P2K2/1P3R2/5r2 b - - am Rxf2",
"1r3rk1/2qbppbp/3p1np1/nP1P2B1/2p2P2/2N1P2P/1P1NB1P1/R2Q1RK1 b - - bm Qb6",
"8/2pN1k2/p4p1p/Pn1R4/3b4/6Pp/1P3K1P/8 w - - bm Ke1",
"5r1k/1p4bp/3p1q2/1NpP1b2/1pP2p2/1Q5P/1P1KBP2/r2RN2R b - - bm f3",
"r3kb1r/pbq2ppp/1pn1p3/2p1P3/1nP5/1P3NP1/PB1N1PBP/R2Q1RK1 w kq - bm a3",
"5rk1/n2qbpp1/pp2p1p1/3pP1P1/PP1P3P/2rNPN2/R7/1Q3RK1 w - - bm h5",
"r5k1/1bqp1rpp/2n1p3/1p4p1/1b2PP2/2NBB1P1/PPPQ4/2KR3R w - - bm a3",
"1r4k1/1nq3pp/pp1pp1r1/8/PPP2P2/6P1/5N1P/2RQR1K1 w - - bm f5",
"q5k1/p2p2bp/1p1p2r1/2p1np2/6p1/1PP2PP1/P2PQ1KP/4R1NR b - - bm Qd5",
"r4rk1/ppp2ppp/1nnb4/8/1P1P3q/PBN1B2P/4bPP1/R2QR1K1 w - - bm Qxe2",
"1r3k2/2N2pp1/1pR2n1p/4p3/8/1P1K1P2/P5PP/8 w - - bm Kc4",
"6r1/6r1/2p1k1pp/p1pbP2q/Pp1p1PpP/1P1P2NR/1KPQ3R/8 b - - bm Qf5",
"r1b1kb1r/1p1npppp/p2p1n2/6B1/3NPP2/q1N5/P1PQ2PP/1R2KB1R w Kkq - bm Bxf6",
"r3r1k1/1bq2ppp/p1p2n2/3ppPP1/4P3/1PbB4/PBP1Q2P/R4R1K w - - bm gxf6",
"r4rk1/ppq3pp/2p1Pn2/4p1Q1/8/2N5/PP4PP/2KR1R2 w - - bm Rxf6",
"r1bqr1k1/3n1ppp/p2p1b2/3N1PP1/1p1B1P2/1P6/1PP1Q2P/2KR2R1 w - - bm Qxe8",
"5rk1/1ppbq1pp/3p3r/pP1PppbB/2P5/P1BP4/5PPP/3QRRK1 b - - bm Bc1",
"r3r1kb/pbq2pp1/1n1p1npB/5NQ1/2p1P1p1/2N2P2/PPP5/2KR3R w - - bm Bg7",
"8/3P4/1p3b1p/p7/P7/1P3NPP/4p1K1/3k4 w - - bm g4",
"3q1rk1/7p/rp1n4/p1pPbp2/P1P2pb1/1QN4P/1B2B1P1/1R3RK1 w - - bm Nb5",
"4r1k1/1r1np3/1pqp1ppB/p7/2b1P1PQ/2P2P2/P3B2R/3R2K1 w - - bm Bg7 Bg5",
"r4r1k/ppq3bp/2R4p/4N1p1/3n4/2N3P1/P4PP1/3QR1K1 w - - bm Ng4",
"r3k2r/pp2pp1p/8/q2Pb3/2P5/4p3/B1Q2PPP/2R2RK1 w kq - bm c5",
"r3r1k1/1bnq1pbn/p2p2p1/1p1P3p/2p1PP1B/P1N2B1P/1PQN2P1/3RR1K1 w - - bm e5",
"8/4k3/p2p2p1/P1pPn2p/1pP1P2P/1P1NK1P1/8/8 w - - bm g4",
"8/2P1P3/b1B2p2/1pPRp3/2k3P1/P4pK1/nP3p1p/N7 w - - bm e8N",
"4K1k1/8/1p5p/1Pp3b1/8/1P3P2/P1B2P2/8 w - - bm f4",
"8/6p1/3k4/3p1p1p/p2K1P1P/4P1P1/P7/8 b - - bm g6, Kc6",
"r1b2rk1/ppp3p1/4p2p/4Qpq1/3P4/2PB4/PPK2PPP/R6R b - - am Qxg2",
"2b1r3/r2ppN2/8/1p1p1k2/pP1P4/2P3R1/4BP1/2K5 w - - bm Nd6",
"2k2Br1/p6b/Pq1r4/1p2p1b1/1Ppp2p1/Q1P3N1/5rPP/R3N1K1 b - - bm Rf6",
"r2qk2r/ppp1b1pp/2n1p3/3pP1n1/3P2b1/2PB1NN1/PP4PP/R1BQK2R w KQkq - bm Nxg5",
"8/8/4p1Pk/1rp1K1p1/4P1P1/1nP2Q2/p2b1P2/8 w - - bm Kf6",
"2k5/p7/Pp1p1b2/1P1P1p2/2P2P1p/3K3P/5B2/8 w - - bm c5",
"8/6pp/5P1k/1p1r4/4R3/7P/5PP1/5K2 w - - am Ke2",
"3q1r1k/4RPp1/p6p/2pn4/2P5/1P6/P3Q2P/6K1 w - - bm Re8",
"rn2k2r/3pbppp/p3p3/8/Nq1Nn3/4B1P1/PP3P1P/R2Q1RK1 w k - bm Nf5",
"r1b1kb1N/pppnq1pB/8/3p4/3P4/8/PPPK1nPP/RNB1R3 b q - bm Ne5",
"N4rk1/pp1b1ppp/n3p1n1/3pP1Q1/1P1N4/8/1PP2PPP/q1B1KB1R b K - bm Nxb4",
"4k1br/1K1p1n1r/2p2pN1/P2p1N2/2P3pP/5B2/P2P4/8 w - - bm Kc8",
"r1bqkb1r/ppp3pp/2np4/3N1p2/3pnB2/5N2/PPP1QPPP/2KR1B1R b kq - bm Ne7",
"r3kb1r/pbqp1pp1/1pn1pn1p/8/3PP3/2PB1N2/3N1PPP/R1BQR1K1 w kq - bm e5",
"r2r2k1/pq2bppp/1np1bN2/1p2B1P1/5Q2/P4P2/1PP4P/2KR1B1R b - - bm Bxf6",
"1r1r2k1/2pq3p/4p3/2Q1Pp2/1PNn1R2/P5P1/5P1P/4R2K b - - bm Rb5",
"8/5p1p/3P1k2/p1P2n2/3rp3/1B6/P4R2/6K1 w - - bm Ba4",
"2rbrnk1/1b3p2/p2pp3/1p4PQ/1PqBPP2/P1NR4/2P4P/5RK1 b - - bm Qxd4",
"4r1k1/1bq2r1p/p2p1np1/3Pppb1/P1P5/1N3P2/1R2B1PP/1Q1R2BK w - - bm c5",
"8/8/8/8/4kp2/1R6/P2q1PPK/8 w - - bm a3"
};

std::ostream& operator<<(std::ostream& os, const Move& move) {
    os.write(reinterpret_cast<const char*>(&move.from), sizeof(move.from));
    os.write(reinterpret_cast<const char*>(&move.to), sizeof(move.to));
    os.write(reinterpret_cast<const char*>(&move.promotion), sizeof(move.promotion));
    os.write(reinterpret_cast<const char*>(&move.isCapture), sizeof(move.isCapture));
    os.write(reinterpret_cast<const char*>(&move.capturedPiece), sizeof(move.capturedPiece));
    return os;
}

// Function to serialize a TT_Entry object
std::ostream& operator<<(std::ostream& os, const TT_Entry& entry) {
    os.write(reinterpret_cast<const char*>(&entry.key), sizeof(entry.key));
    os.write(reinterpret_cast<const char*>(&entry.score), sizeof(entry.score));
    os.write(reinterpret_cast<const char*>(&entry.depth), sizeof(entry.depth));
    os.write(reinterpret_cast<const char*>(&entry.flag), sizeof(entry.flag));
    os << entry.move;
    return os;
}

void saveTranspositionTable(const std::string& filename, Board& board) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    for (const auto& entry : board.transposition_table) {
        file << entry;
    }
}

// Stub for engine move generation
Move getEngineMove1(Board& board, int timeLimit) {
    endTime = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(timeLimit);
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
    double_t bestScore = 0;
    Move bestMove;
    Move prevBestMove;
    int depth = 0;
    std::vector<std::tuple<Move, double_t>> iterativeDeepeningMoves;
    while (depth < 100) {
        depth++;
        std::tie(bestMove, bestScore) = engine(board, depth, iterativeDeepeningMoves, -99999, 99999);

        if (bestScore == -1.2345) { // Timeout
            break;
        }
        else if (bestMove.to != -1 && bestMove.from != -1) { // This is our normal case
            prevBestMove = bestMove;
            if (std::abs(bestScore) > 10000) {
                break;
            }
        }
    }
    if (prevBestMove.from == -1) { // Most likely due to depth 0 failing to do in timelimit
        std::cout << "ERROR ENGINE1: " << depth << std::endl;
        prevBestMove = getEngineMove1(board, timeLimit * 2);
    }
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = currentTime - startTime;    
    std::cout << "MOVE FOUND: engine1 depth: " << depth << " Took: " << elapsed.count() << std::endl;
    return prevBestMove;
}

Move getEngineMove2(Board& board, int timeLimit) {
    endTime2 = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(timeLimit);
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
    double_t bestScore = 0;
    Move bestMove;
    Move prevBestMove;
    double_t inf = 99999;
    int depth = 0;
    std::vector<std::tuple<Move, double_t>> iterativeDeepeningMoves;
    while (depth < 100) {
        depth++;
        std::tie(bestMove, bestScore) = perft2(board, depth, iterativeDeepeningMoves, -inf, inf);

        // Timeout
        if (bestScore == -1.2345) {
            break;
        }
        else if (bestMove.to != -1 && bestMove.from != -1) {
            prevBestMove = bestMove;
            if (std::abs(bestScore) > 10000) {
                return prevBestMove;
            }
        }
    }
    if (prevBestMove.from == -1) {
        std::cout << "ERROR ENGINE2: " << depth << std::endl;
        return getEngineMove2(board, timeLimit * 2);
    }
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = currentTime - startTime;
    std::cout << "engine2 depth: " << depth << " Took: " << elapsed.count() << std::endl;    return prevBestMove;
}

// ASPIRATION WINDOWS NEEDS MORE TESTING
/**
Move getEngineMove2(Board& board, int timeLimit) {
    endTime2 = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(timeLimit);
    double_t bestScore = 0;
    double_t prevBestScore = 0;
    Move bestMove;
    Move prevBestMove;
    Move ultimateMove;
    double_t inf = 99999;
    int depth = 0;
    std::vector<std::tuple<Move, double_t>> iterativeDeepeningMoves;
    while (depth < 100) {
        depth++;
        double_t delta = 40000;
        ultimateMove = prevBestMove;
        if (std::abs(prevBestScore) > 10000) {
            break;
        }
        double_t alpha = std::max(prevBestScore - delta, -inf);
        double_t beta = std::min(prevBestScore + delta, inf);
        while (true) {
            std::tie(bestMove, bestScore) = perft2(board, depth, iterativeDeepeningMoves, alpha, beta);

            // Timeout
            if (bestScore == -1.2345) {
                break;
            }
            else if (bestMove.to != -1 && bestMove.from != -1) {
                prevBestMove = bestMove;
                prevBestScore = bestScore;
                if (bestScore <= alpha) {
                    beta = (alpha + beta) / 2;
                    alpha = std::max(bestScore - delta, -inf);
                }
                else if (bestScore >= beta) {
                    beta = std::min(bestScore + delta, inf);
                }
                else {
                    break;
                }
                delta += delta;
            }
            else {
                std::cout << "MINOR ERROR ENGINE1" << std::endl;
            }
        }
        if (bestScore == -1.2345) {
            break;
        }
    }
    if (ultimateMove.from == -1) { 
        std::cout << "MAJOR ERROR ENGINE1" << std::endl;
        ultimateMove = getEngineMove2(board, timeLimit * 2);
    }
    //std::cout << "engine2 depth: " << depth << std::endl;
    return ultimateMove;
}
*/

bool isDrawByRepetition(const std::deque<std::pair<int, int>>& moves) {
    if (moves.size() < 6) return false;
    for (int i = 0; i <= moves.size() - 6; i += 2) {
        if (moves[i] == moves[i + 4] && moves[i + 1] == moves[i + 5]) {
            return true;
        }
    }
    return false;
}

void playEngines(int& engine1Wins, int& engine2Wins, int& draws, int timeLimit, bool displayOn, int moveLimit) {
    auto playSingleGame = [&](bool engine1First, Board& board, BoardDisplay& display, sf::RenderWindow& window) {
        bool isEngine1Turn = engine1First;
        int numMoves = 0;

        while ((displayOn && window.isOpen()) || !displayOn) {
            if (displayOn) {
                sf::Event event;
                while (window.pollEvent(event)) {
                    if (event.type == sf::Event::Closed)
                        window.close();
                }
            }

            if (board.generateAllMoves().empty()) {
                if (board.amIInCheck(board.whiteToMove)) {
                    if (isEngine1Turn) {
                        engine2Wins++;
                    }
                    else {
                        engine1Wins++;
                    }
                }
                else {
                    draws++;
                    std::cout << "stalemate" << std::endl;

                }
                std::cout << "engine1 wins: " << engine1Wins << " engine2 wins: " << engine2Wins << " draws: " << draws << std::endl;
                break;
            }
            else if (board.isThreefoldRepetition(false)) {
                draws++;
                std::cout << "3fold draw" << std::endl;
                std::cout << "engine1 wins: " << engine1Wins << " engine2 wins: " << engine2Wins << " draws: " << draws << std::endl;
                break;
            }

            Move engineMove = isEngine1Turn ? getEngineMove1(board, timeLimit) : getEngineMove2(board, timeLimit);
            board.makeMove(engineMove);
            board.lastMove = engineMove;
            numMoves++;
            board.updatePositionHistory(true);

            if (displayOn) {
                display.updatePieces(window, board);
            }
            // Check for draw condition based on insufficient material
            if ((std::_Popcount(board.whitePieces) == 1) && (std::_Popcount(board.blackPieces) == 1)) {
                draws++;
                std::cout << "insufficient material draw" << std::endl;

                std::cout << "engine1 wins: " << engine1Wins << " engine2 wins: " << engine2Wins << " draws: " << draws << std::endl;
                break;
            }

            else if (numMoves > moveLimit) {
                draws++;
                std::cout << "move limit draw" << std::endl;

                std::cout << "engine1 wins: " << engine1Wins << " engine2 wins: " << engine2Wins << " draws: " << draws << std::endl;
                break;
            }

            isEngine1Turn = !isEngine1Turn;

            if (displayOn) {
                window.clear();
                display.draw(window);
                window.display();
            }
        }
        };

    for (auto fen : fenArray) {
        // First game: Engine 1 starts
        initializeZobristTable();
        Board board1;
        board1.createBoardFromFEN(fen);
        board1.configureTranspositionTableSize(128);  // Example of setting the TT size to 128 MB

        BoardDisplay display1;
        sf::RenderWindow window1;

        if (displayOn) {
            display1.setupPieces(board1);
            window1.create(sf::VideoMode(480, 480), "Chess Board");
        }

        playSingleGame(true, board1, display1, window1);

        // Second game: Engine 2 starts
        initializeZobristTable();
        Board board2;
        board2.createBoardFromFEN(fen);
        board2.configureTranspositionTableSize(128);  // Example of setting the TT size to 128 MB

        BoardDisplay display2;
        sf::RenderWindow window2;

        if (displayOn) {
            display2.setupPieces(board2);
            window2.create(sf::VideoMode(480, 480), "Chess Board");
        }

        playSingleGame(false, board2, display2, window2);
    }
}

void playAgainstComputer(char playerColor, int timeLimit) {
    initializeZobristTable();
    Board board;
    board.createBoard();
    //board.createBoardFromFEN("1k1r3r/2p2qpp/R5b1/2P5/1P1pP3/3Bb2P/6Q1/R6K w - - 0 1");


    BoardDisplay display;
    display.setupPieces(board);

    sf::RenderWindow window(sf::VideoMode(480, 480), "Chess Board");

    bool isPlayerTurn = (playerColor == 'w');

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (isPlayerTurn) {
            // Check for game end
            if (board.generateAllMoves().empty()) {
                if (board.amIInCheck(board.whiteToMove)) {
                    std::cout << "Engine wins!" << std::endl;
                }
                else {
                    std::cout << "Draw by stalemate!" << std::endl;
                }
                break;
            }
            else if (board.isThreefoldRepetition(false)) {
                std::cout << "Draw by repitition!" << std::endl;
                break;
            }

            if (display.handleMove(window, board)) {
                board.updatePositionHistory(true);
                std::cout << "Player move made" << std::endl;
                isPlayerTurn = false;
            }
        }
        else {
            // Check for game end
            if (board.generateAllMoves().empty()) {
                if (board.amIInCheck(board.whiteToMove)) {
                    std::cout << "Player wins!" << std::endl;
                }
                else {
                    std::cout << "Draw by stalemate!" << std::endl;
                }
                break;
            }
            else if (board.isThreefoldRepetition(false)) {
                std::cout << "Draw by repitiion!" << std::endl;
                break;
            }

            Move engineMove = getEngineMove2(board, timeLimit);
            board.makeMove(engineMove);
            std::cout << engineMove.from << engineMove.to << std::endl;
            board.lastMove = engineMove;
            board.updatePositionHistory(true);
            board.printBoard();
            display.updatePieces(window, board);
            std::cout << "Engine move made" << std::endl;
            isPlayerTurn = true;
        }

        window.clear();
        display.draw(window);
        window.display();
    }
}

int main() {
    //Move move = convertToMoveObject("e2e4");
    int engine1Wins = 0;
    int engine2Wins = 0;
    int draws = 0;
    int timeLimit = 400; //milliseconds
    char playerColour = 'w';
    bool display = false;
    int moveLimit = 150;
    //playAgainstComputer(playerColour, timeLimit);
    playEngines(engine1Wins, engine2Wins, draws, timeLimit, display, moveLimit);

    std::cout << "Engine 1 wins: " << engine1Wins << std::endl;
    std::cout << "Engine 2 wins: " << engine2Wins << std::endl;
    std::cout << "Draws: " << draws << std::endl;
    return 0;
}


Move parseMove(const std::string& moveStr, Board& board) {
    // King side castle
    if (moveStr == "'O-O'") {
        if (board.whiteToMove) {
            Move move(3, 1);
            return move;
        }
        else {
            Move move(59, 57);
            return move;
        }
    }
    else if (moveStr == "'O-O-O'") {
        if (board.whiteToMove) {
            Move move(3, 5);
            return move;
        }
        else {
            Move move(59, 61);
            return move;
        }
    }
    bool isCapture = moveStr.find('x') != std::string::npos;
    char promotion = 0;
    int startIndex = 1;
    int expectedLength = 4;
    if (isCapture) {
        expectedLength += 1;
        startIndex++;
    }
    if (moveStr.find('=') != std::string::npos) {
        promotion = moveStr.back();
        expectedLength += 2;
    }

    // Simplified example, does not handle disambiguation or pawn moves accurately
    std::string toPos;
    char pieceMoving = 'P';

    if (isupper(moveStr[1])) {
        // For non-pawn moves
        startIndex++;
        expectedLength++;
        pieceMoving = moveStr[1];
    }

    int file = -1;
    int rank = -1;
    // checking disambiguation
    if (size(moveStr) > expectedLength) {
        char disambig = isupper(moveStr[1]) ? moveStr[2] : moveStr[1];
        if (isalpha(disambig)) {
            file = 'h' - disambig;
        }
        else {
            rank = disambig - '1';
        }
        startIndex++;
    }
   
    toPos = moveStr.substr(startIndex, 2);
    int toIndex = boardPositionToIndex(toPos);

    std::vector<Move> moves;
    Bitboard ownPieces = board.whiteToMove ? board.whitePieces : board.blackPieces;
    Bitboard opponentPieces = board.whiteToMove ? board.blackPieces : board.whitePieces;
    if (pieceMoving == 'P') {
        moves = board.generatePawnMoves(board.whiteToMove ? board.whitePawns : board.blackPawns, ownPieces, opponentPieces);
    } 
    else if (pieceMoving == 'N') {
        moves = board.generateKnightMoves(board.whiteToMove ? board.whiteKnights : board.blackKnights, ownPieces, opponentPieces);
    }
    else if (pieceMoving == 'B') {
        moves = board.generateBishopMoves(board.whiteToMove ? board.whiteBishops : board.blackBishops, ownPieces, opponentPieces);
    }
    else if (pieceMoving == 'R') {
        moves = board.generateRookMoves(board.whiteToMove ? board.whiteRooks : board.blackRooks, ownPieces, opponentPieces);
    }
    else if (pieceMoving == 'Q') {
        moves = board.generateQueenMoves(board.whiteToMove ? board.whiteQueens : board.blackQueens, ownPieces, opponentPieces);
    }
    else if (pieceMoving == 'K') {
        moves = board.generateKingMoves(board.whiteToMove ? board.whiteKing : board.blackKing, ownPieces, opponentPieces);
    }

    std::vector<Move> legalMoves;
    Bitboard store = board.enPassantTarget;
    bool whiteKingMovedStore = board.whiteKingMoved;
    bool whiteLRookMovedStore = board.whiteLRookMoved;
    bool whiteRRookMovedStore = board.whiteRRookMoved;
    bool blackKingMovedStore = board.blackKingMoved;
    bool blackLRookMovedStore = board.blackLRookMoved;
    bool blackRRookMovedStore = board.blackRRookMoved;
    for (Move& move : moves) {
        board.makeMove(move);
        if (!board.amIInCheck(!board.whiteToMove)) {
            legalMoves.push_back(move);
        }
        board.enPassantTarget = store;
        board.whiteKingMoved = whiteKingMovedStore;
        board.whiteLRookMoved = whiteLRookMovedStore;
        board.whiteRRookMoved = whiteRRookMovedStore;
        board.blackKingMoved = blackKingMovedStore;
        board.blackLRookMoved = blackLRookMovedStore;
        board.blackRRookMoved = blackRRookMovedStore;
        board.undoMove(move);

    }

    for (Move move : legalMoves) {

        if (move.to == toIndex) {
            if ((file == -1) && (rank == -1)) {
                return move;
            }
            else if ((rank == -1) && (move.from % 8 == file)) {
                return move;
            }
            else if ((file == -1) && (move.from / 8 == rank)) {
                return move;
            }
        }
    }
    return Move();
}

void convertMoves(const std::string& input, Board& board) {
    std::vector<Move> moveList;
    std::istringstream inputStream(input);
    std::string line;
    while (std::getline(inputStream, line)) {
        if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
            board.createBoard();
            std::vector<std::string> movesVec;
            std::string movesStr = line.substr(line.find('{') + 1, line.find('}') - line.find('{') - 1);
            std::istringstream movesStream(movesStr);
            std::string move;
            while (std::getline(movesStream, move, ',')) {
                move.erase(remove_if(move.begin(), move.end(), isspace), move.end()); // Remove spaces
                movesVec.push_back(move);
            }

            for (const auto& moveStr : movesVec) {


                Move move = parseMove(moveStr, board);
                if (move.from == -1) {
                    
                    std::cout << "ERROR" << std::endl;
                    break;
                }
                uint64_t hash = board.generateZobristHash();
                TT_Entry* ttEntry = board.probeTranspositionTable(hash);
                board.record_tt_entry(hash, 0, HASH_BOOK, move, 0);
                board.makeMove(move);
            }
        }
    }
}


int main32() {
    Board board;
    initializeZobristTable();
    std::ifstream file("StartingMoves.txt");
    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return 1;
    }

    std::string line;
    std::string buffer;
    int lineCount = 0;
    const int chunkSize = 1;

    while (std::getline(file, line)) {
        buffer += line + "\n";
        lineCount++;

        if (lineCount == chunkSize) {
            convertMoves(buffer, board);
            buffer.clear();
            lineCount = 0;
        }
    }

    // Process any remaining lines if they exist
    if (!buffer.empty()) {
        convertMoves(buffer, board);
    }
    std::cout << "Number of entries in the transposition table: " << board.countTranspositionTableEntries() << std::endl;

    // Save the transposition table to a file
    saveTranspositionTable("transposition_table.dat", board);

    file.close();
    return 0;
}