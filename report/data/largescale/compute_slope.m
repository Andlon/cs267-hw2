function [ slope, slopes ] = compute_slope(file)
%% Computes slope in the sense O(n^slope) for the ASCII data given in file
%  (summary outputs from particle simulations)
    data = load(file, '-ascii');
    timecol = size(data, 2);
    n = data(:, 1);
    t = data(:, timecol);
   
    slopes = diff(log(t)) ./ diff(log(n));
    logn = log(n);
    logt = log(t);
    coeff = polyfit(logn, logt, 1);
    slope = coeff(1);
end