%% MatlabFileTest
% We want to verify that our C++ wrapper for HDF5 can read and write .mat
% files.  Check your preferences and make sure that version 7.3+ file
% format is selected

%% Generate some data

SIZE = 2048;

% a_1d_R = randn(SIZE, 1, 'single');
% a_1d_D = randn(SIZE, 1, 'double');
% a_1d_C = randn(SIZE, 1, 'single') + 1j*randn(SIZE, 1, 'single');
a_1d_C = single(10*ones(SIZE, 1)) + 1j*single(10*ones(SIZE, 1));
% a_1d_Z = randn(SIZE, 1, 'double') + 1j*randn(SIZE, 1, 'double');

% a_2d_R = randn(SIZE, SIZE, 'single');
% a_2d_D = randn(SIZE, SIZE, 'double');
% a_2d_C = randn(SIZE, SIZE, 'single') + 1j*randn(SIZE, SIZE, 'single');
% a_2d_Z = randn(SIZE, SIZE, 'double') + 1j*randn(SIZE, SIZE, 'double');

whos

%% Save
% save('example_input.mat', ...
%     'a_1d_R', 'a_1d_D', 'a_1d_C', 'a_1d_Z', ...
%     'a_2d_R', 'a_2d_D', 'a_2d_C', 'a_2d_Z');


save('example_input.mat', 'a_1d_C');


%% Run the test app which will decimate the values
[status, stdout] = system('../../build/MatlabFileTestApp');

%% Load
% Should have b variables:
% 
% load('example_output.mat', ...
%     'b_1d_R', 'b_1d_D', 'b_1d_C', 'b_1d_Z', ...
%     'b_2d_R', 'b_2d_D', 'b_2d_C', 'b_2d_Z');

load('example_output.mat', ...
    'b_1d_C');

%% Verify
% c_1d_R = a_1d_R / 10;
% c_1d_D = a_1d_D / 10;
c_1d_C = a_1d_C / 10;
% c_1d_Z = a_1d_Z / 10;

% c_2d_R = a_2d_R / 10;
% c_2d_D = a_2d_D / 10;
% c_2d_C = a_2d_C / 10;
% c_2d_Z = a_2d_Z / 10;

%%
% disp(['1-Dimensional Real          : ' num2str(norm(c_1d_R - b_1d_R))]);
% disp(['1-Dimensional Double        : ' num2str(norm(c_1d_D - b_1d_D))]);
disp(['1-Dimensional Complex Single: ' num2str(norm(c_1d_C - b_1d_C))]);
% disp(['1-Dimensional Complex Double: ' num2str(norm(c_1d_Z - b_1d_Z))]);

% disp(['2-Dimensional Real          : ' num2str(norm(c_2d_R - b_2d_R))]);
% disp(['2-Dimensional Double        : ' num2str(norm(c_2d_D - b_2d_D))]);
% disp(['2-Dimensional Complex Single: ' num2str(norm(c_2d_C - b_2d_C))]);
% disp(['2-Dimensional Complex Double: ' num2str(norm(c_2d_Z - b_2d_Z))]);




