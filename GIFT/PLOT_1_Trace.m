function traces = load_trace_trace(trace_list, ta_path)

clc;
close all;
clear all;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Trace Plotting
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%% ���� �Է� ���� %%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Plotting �� ���� ���� ���� (0 ���� ����)
start_tr = 0;
% Plotting �� ���� �� (1 ���� ����)
tr_num = 1;
% Plotting �� ���� ����
ta_path = 'E:\ChipWhisperer_Normal_ARIA_1R_1000tr_2730pt\ChipWhisperer_Normal_ARIA_1R_1000tr_2730pt.trace';
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

trace_list = [1:tr_num];

if isempty(trace_list)
    traces = [];
    error('you need to enter list of trace numbers')
    return;
end

fid = fopen(ta_path, 'rb');

% Read Header
char(fread(fid, 20, 'int8'))'

metadata = fread(fid, 3, 'int32');
num = metadata(1);
len = metadata(2);
Traces_Num = num
Traces_len = len

traces = zeros(len, length(trace_list));

for ii = 1 : length(trace_list)
    istheretrace = fseek(fid, 32 + (len * (trace_list(ii) - 1 + start_tr) * 4), 'bof');
    if istheretrace == -1
        traces = [];
        error('you need to enter trace numbers same or less than maximum number')
        break;
    end
    tt = fread(fid, len, 'float');
    traces(:,ii) = tt;
end

fclose(fid);

plot(traces,'DisplayName','ans')
set(gca,'fontname','Times New Roman');
xlabel('Time','fontsize',16,'fontweight','bold','fontname','Times New Roman');
ylabel('Power Consumption','fontsize',16,'fontweight','bold','fontname','Times New Roman');

end
