%[2] i = 0 / j = 2
%[2] Step = 0
%[2] k = 0
%[2] Wait for T from 0
%[3] i = 1 / j = 0
%[4] i = 1 / j = 1
%[4] Step = 0
%[4] k = 1
%[4] Broadcast A : i = 1 / j = 1 / k =  1 from 1 (step=0)
%[4] After broadcast A : i = 1 / j = 1 / k =  1 from 1
%[4] Before add
%[5] i = 1 / j = 2
%[5] Step = 0
%[5] k = 1
%[5] Wait for T from 1
%[4] After add
%[4] Step = 1
%[4] k = 2
%[4] Wait for T from 2
%[6] i = 2 / j = 0
%[6] Step = 0
%[6] k = 2
%[6] Wait for T from 2
%[6] Receive T from 2
%[6] Before add
%[6] After add
%[6] Step = 1
%[6] k = 0
%[8] i = 2 / j = 2
%[8] Step = 0
%[8] k = 2
%[8] Broadcast A : i = 2 / j = 2 / k =  2 from 2 (step=0)
%[8] After broadcast A : i = 2 / j = 2 / k =  2 from 2
%[8] Before add
%[8] After add
%[8] Step = 1
%[8] k = 0
%[8] Wait for T from 0
%[6] Broadcast A : i = 2 / j = 0 / k =  0 from 0 (step=1)
%[6] After broadcast A : i = 2 / j = 0 / k =  0 from 0
%[6] Send S to 3
%[1] i = 0 / j = 1
%[1] Step = 0
%[1] k = 0
%[1] Wait for T from 0
%[6] i =  2 / j = 0 / step = 1
%[5] Receive T from 1
%[5] Before add
%[5] After add
%[5] Step = 1
%[5] k = 2
%[5] Broadcast A : i = 1 / j = 2 / k =  2 from 2 (step=1)
%[6] Wait for S from 0
%[5] After broadcast A : i = 1 / j = 2 / k =  2 from 2
%[5] Send S to 2
%[5] i =  1 / j = 2 / step = 1
%[4] Receive T from 2
%[4] Send S to 1
%[4] i =  1 / j = 1 / step = 1
%[5] Wait for S from 8
%[4] Wait for S from 7
%[8] Receive T from 0
%[8] Send S to 5
%[8] i =  2 / j = 2 / step = 1
%[8] Wait for S from 2
%[5] Received S from 8
%[5] Before add
%[5] After add
%[5] Step = 2
%[5] k = 0
%[5] Wait for T from 0
%[3] Step = 0
%[3] k = 1
%[3] Wait for T from 1
%[3] Receive T from 1
%[3] Before add
%[3] After add
%[3] Step = 1
%[3] k = 2
%[3] Wait for T from 2
%[3] Receive T from 2
%[3] Send S to 0
%[3] i =  1 / j = 0 / step = 1
%[3] Wait for S from 6
%[3] Received S from 6
%[3] Before add
%[3] After add
%[3] Step = 2
%[3] k = 0
%[3] Broadcast A : i = 1 / j = 0 / k =  0 from 0 (step=2)
%[5] Receive T from 0
%[5] Send S to 2
%[5] i =  1 / j = 2 / step = 2
%[3] After broadcast A : i = 1 / j = 0 / k =  0 from 0
%[5] Wait for S from 8
%[3] Send S to 0
%[3] i =  1 / j = 0 / step = 2
%[3] Wait for S from 6
%[0] i = 0 / j = 0
%[7] i = 2 / j = 1
%[7] Step = 0
%[7] k = 2
%[0] Step = 0
%[7] Wait for T from 2
%[0] k = 0
%[7] Receive T from 2
%[0] Broadcast A : i = 0 / j = 0 / k =  0 from 0 (step=0)
%[1] Receive T from 0
%[1] Before add
%[2] Receive T from 0
%[2] Before add
%[2] After add
%[0] After broadcast A : i = 0 / j = 0 / k =  0 from 0
%[2] Step = 1
%[2] k = 1
%[1] After add
%[1] Step = 1
%[1] k = 1
%[2] Wait for T from 1
%[2] Receive T from 1
%[2] Send S to 8
%[1] Broadcast A : i = 0 / j = 1 / k =  1 from 1 (step=1)
%[1] After broadcast A : i = 0 / j = 1 / k =  1 from 1
%[1] Send S to 7
%[2] i =  0 / j = 2 / step = 1
%[1] i =  0 / j = 1 / step = 1
%[1] Wait for S from 4
%[2] Wait for S from 5
%[2] Received S from 5
%[2] Before add
%[8] Received S from 2
%[8] Before add
%[1] Received S from 4
%[1] Before add
%[1] After add
%[1] Step = 2
%[8] After add
%[8] Step = 2
%[8] k = 1
%[8] Wait for T from 1
%[2] After add
%[2] Step = 2
%[2] k = 2
%[2] Broadcast A : i = 0 / j = 2 / k =  2 from 2 (step=2)
%[1] k = 2
%[1] Wait for T from 2
%[1] Receive T from 2
%[2] After broadcast A : i = 0 / j = 2 / k =  2 from 2
%[2] Send S to 8
%[2] i =  0 / j = 2 / step = 2
%[1] Send S to 7
%[1] i =  0 / j = 1 / step = 2
%[1] Wait for S from 4
%[2] Wait for S from 5
%[2] Received S from 5
%[2] Before add
%[2] After add
%[7] Before add
%[7] After add
%[7] Step = 1
%[0] Before add
%[7] k = 0
%[0] After add
%[7] Wait for T from 0
%[0] Step = 1
%[7] Receive T from 0
%[0] k = 1
%[0] Wait for T from 1
%[0] Receive T from 1
%[7] Send S to 4
%[7] i =  2 / j = 1 / step = 1
%[7] Wait for S from 1
%[4] Received S from 7
%[4] Before add
%[4] After add
%[4] Step = 2
%[4] k = 0
%[4] Wait for T from 0
%[4] Receive T from 0
%[4] Send S to 1
%[4] i =  1 / j = 1 / step = 2
%[4] Wait for S from 7
%[1] Received S from 4
%[1] Before add
%[1] After add
%[7] Received S from 1
%[7] Before add
%[7] After add
%[0] Send S to 6
%[7] Step = 2
%[0] i =  0 / j = 0 / step = 1
%[7] k = 1
%[0] Wait for S from 3
%[6] Received S from 0
%[6] Before add
%[6] After add
%[6] Step = 2
%[6] k = 1
%[6] Wait for T from 1
%[7] Broadcast A : i = 2 / j = 1 / k =  1 from 1 (step=2)
%[6] Receive T from 1
%[8] Receive T from 1
%[8] Send S to 5
%[8] i =  2 / j = 2 / step = 2
%[8] Wait for S from 2
%[7] After broadcast A : i = 2 / j = 1 / k =  1 from 1
%[7] Send S to 4
%[7] i =  2 / j = 1 / step = 2
%[8] Received S from 2
%[8] Before add
%[8] After add
%[0] Received S from 3
%[0] Before add
%[0] After add
%[0] Step = 2
%[7] Wait for S from 1
%[7] Received S from 1
%[7] Before add
%[7] After add
%[0] k = 2
%[0] Wait for T from 2
%[0] Receive T from 2
%[4] Received S from 7
%[4] Before add
%[4] After add
%[0] Send S to 6
%[0] i =  0 / j = 0 / step = 2
%[0] Wait for S from 3
%[0] Received S from 3
%[0] Before add
%[0] After add
%[5] Received S from 8
%[5] Before add
%[5] After add
%[6] Send S to 3
%[6] i =  2 / j = 0 / step = 2
%[6] Wait for S from 0
%[6] Received S from 0
%[6] Before add
%[6] After add
%[3] Received S from 6
%[3] Before add
%[3] After add
%matrice complete A
A = [-5 7 9 8 0 5 -1 6 -2 ;-3 9 9 -3 6 8 -8 -3 5 ;-5 1 3 -3 2 2 -9 -5 2 ;1 8 7 6 9 0 0 8 -4 ;9 6 -9 -8 -10 1 -9 -2 1 ;-8 -9 -10 -7 -4 1 3 -8 -3 ;-8 4 4 3 9 6 -9 -2 -10 ;4 -6 -8 -1 3 -9 -2 -10 3 ;-6 9 5 -8 -7 -7 5 2 -4 ]
%matrice complete B
B = [9 -4 7 -5 6 -10 2 5 4 ;-6 -7 9 -10 6 2 -5 7 -6 ;1 -3 -9 -8 3 8 -8 -7 -8 ;-3 -9 -5 7 -4 -9 -6 7 -10 ;9 6 -8 -8 8 -8 -5 -4 -1 ;8 -2 7 -7 0 0 -5 -4 9 ;-9 0 -6 5 2 -1 7 -3 -5 ;-3 -1 -6 -9 -9 2 -6 5 9 ;-1 -5 -3 9 8 4 -3 -6 5 ]
%matrice complete C
C = [-47 -32 65 -39 24 6 -16 51 -24 ;-49 38 -128 30 -51 114 -41 -96 -54 ;-49 65 -126 79 -78 96 -9 -94 -22 ;125 25 -20 -106 60 -73 -81 -53 45 ;-45 -27 -156 78 12 -129 -39 75 -177 ;-137 38 42 86 -40 144 143 20 -1 ;-60 -39 -90 -12 -27 39 -51 0 96 ;9 42 15 -113 -101 -29 2 72 -13 ;100 42 111 -40 -9 -34 7 31 -58 ]
