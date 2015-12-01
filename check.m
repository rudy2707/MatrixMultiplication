% Axel Fahy - 01.12.2015
% Verification of matrix multiplication
% for parallele programming.

out;    % Import out.m
%A = importdata("A.txt")
%B = importdata("B.txt")
%C = importdata("C.txt")

R = A * B

if C == R
    disp("CORRECT")
else
    disp("FALSE")
end


%FILENAME = "out.txt"
%
%
%line_number = 8;
%line = '';
%file = fopen(FILENAME, 'r');
%
%if (file < 0)
%    printf('Error: could not open the file\n')
%else
%    n = 0
%    while ~feof(file), n = n+1;
%        if (n ~= line_number)
%            fgetl(file);
%        else
%            A = fgetl(file);
%            %A = textscan(file, '%f')
%            %line = fgetl(file);
%            %B = fgetl(file);
%            %line = fgetl(file);
%            %C = fgetl(file);
%            %line = fgetl(file);
%        end;
%    end;
%    fclose(file);
%end;
%
%disp("A")
%A
%size(A)
%A = [1 2 3; 4 5 6]
%size(A)
%disp("B")
%disp(B)
%disp("C")
%disp(C)


%disp("R")
%size(A)
%size(B)
%R = A * B
%
%if C == R
%    disp("CORRECT")
%else
%    disp("FALSE")
%end


data = importdata(FILENAME, '', 7)
data

