%servo_IR_ML
% goes with HW14_ctd27

% R.Kuc
% 10/18/2014
s = serial('COM3','BaudRate',57600); 
% must be same COM# and BR as Monitor
fopen(s) % open serial port
fprintf(1,'Connecting *'); 
% needs delay for reliable connection
for i=1:3 
 pause(1) % 3 seconds 
 fprintf(1,'*'); 
end
fprintf(1,'\n'); 

servoPos = 0;
range = 0;
i = 0;

n = input('input angle= '); 
for j = 1:19
    d1 = fscanf(s);
end
fprintf(s,'%d',n); % send string to UNO
d1 = fscanf(s)

while(servoPos < n)
    i = i+1;
     
    d2 = fscanf(s)
    % reads char str from UNO
   % FromUNO = str2num(d2)
    servoPos(i) = str2num(d2) 
    % converts char str into numbers
    d3 = fscanf(s);
    val = str2num(d3);
    range(i)=34.8*(1/(5*val/1023))-6.88 
    % range in cm
end

fclose(s) % close serial port
plot(servoPos,range)
xlabel('servo position (degrees)')
ylabel('range (cm)')
