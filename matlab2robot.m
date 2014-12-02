% matlab2robot.m
% Christopher Datsikas, Hope Wilson, Bear Flintgruber
% Parking Robot - Final Project for EE 348
% RC robot movement controlled by Matlab (Visual observation of robot)

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

while(1)
    Rmotor = input('input right motor speed (0-1023): ');
    fprintf(s,'%d',Rmotor); % send string to UNO
    Lmotor = input('input left motor speed (0-1023): ');
    fprintf(s, '%d', Lmotor); % send string to UNO
    direct = input('input direction (0 for backwards, 1 for forwards): ');
    fprintf(s, '%d', direct); % send string to UNO
end
    
    