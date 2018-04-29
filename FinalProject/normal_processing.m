%%
%%
%%

clear;
close;
clc;

depth_img = imread('camera_0d600f92f8d14e288ddc590c32584a5a_conferenceRoom_1_frame_0_domain_depth.png');

% read files in folder with specific suffix, such as *.jpg or *.png
allFiles = dir('L:\Photogrammetry\Datasets\3dsemantics\noXYZ\noXYZ%2Farea_1_no_xyz\area_1\data\normal\scene\conferenceRoom\2\*.png');
allNames = {allFiles.name};
N = length(allFiles);

normalLabel = zeros(N, 360, 480);

for i = 1:N
    % get image path and name 
    % read image into memory
    image_name = fullfile(allFiles(i).folder, allFiles(i).name);
    img = imread(image_name);
    
    new_img = zeros(1080, 1080, 3, 'double');
    new_label = zeros(1080, 1080, 'double');
    
    % operate image and write to file
    for m = 1:size(img, 1)
        for n = 1:size(img, 2)
            pixel = img(m,n,:);
            pixel = double(reshape(pixel, [3,1]));
            
            if (pixel(1)==128 && pixel(2)==128 && pixel(3)==128)
                new_img(m,n,1) = 1;
                new_img(m,n,2) = 0;
                new_img(m,n,3) = 0;
                
                % surface normal
                new_label(m,n) =0;
            else
                pixel_new = pixel - 127.5;
                r = sqrt(pixel_new(1)^2 + pixel_new(2)^2 + pixel_new(3)^2);
        
                x = pixel_new(1)/r;
                y = pixel_new(2)/r;
                z = pixel_new(3)/r;
                
                theta = atan2(y, x);
                phi = acos(z);
                
                % transformation between degree and radian
                if y > 0
                    theta_d = round(rad2deg(theta));
                else
                    theta_d = 360 + round(rad2deg(theta));
                end
                
                phi_d = round(rad2deg(phi));
                
                new_img(m,n,1) = 1;
                new_img(m,n,2) = theta_d;
                new_img(m,n,3) = phi_d;
                
               
                %% transform theta and phi_d to label
                %  missing surface normal (128, 128, 128) as (1,0,0)
                % 
                %  divide sphere into 18 parts
                %  theta:
                %          0~60, 60~120, 120~180, 180~240, 240~300, 300~360 
                %  phi_d:     
                %  0~60     1      4        7       10       13       16
                %  60~120   2      5        8       11       14       17 
                %  120~180  3      6        9       12       15       18
                %
                %  missing surface normal is 0
                % 1 part
                if ((theta_d>=0) && (theta_d<60))
                    if ((phi_d>=0) && (phi_d<60))
                        new_label(m,n) =1;
                    elseif ((phi_d>=60) && (phi_d<120))
                        new_label(m,n) =2;
                    elseif ((phi_d>=120) && (phi_d<=180))
                        new_label(m,n) =3;
                    end
                end
                
                
                % 2 part
                if ((theta_d>=60) && (theta_d<120))
                    if (phi_d>=0) && (phi_d<60)
                        new_label(m,n) =4;
                    elseif (phi_d>=60) && (phi_d<120)
                        new_label(m,n) =5;
                    elseif (phi_d>=120) && (phi_d<=180)
                        new_label(m,n) =6;
                    end
                end
                
                % 3 part
                if ((theta_d>=120) && (theta_d<180))
                    if (phi_d>=0) && (phi_d<60)
                        new_label(m,n) =7;
                    elseif (phi_d>=60) && (phi_d<120)
                        new_label(m,n) =8;
                    elseif (phi_d>=120) && (phi_d<=180)
                        new_label(m,n) =9;
                    end
                end
                
                
                % 4 part
                if ((theta_d>=180) && (theta_d<240))
                    if (phi_d>=0) && (phi_d<60)
                        new_label(m,n) =10;
                    elseif (phi_d>=60) && (phi_d<120)
                        new_label(m,n) =11;
                    elseif (phi_d>=120) && (phi_d<=180)
                        new_label(m,n) =12;
                    end
                end
                
                
                % 5 part
                if ((theta_d>=240) && (theta_d<300))
                    if (phi_d>=0) && (phi_d<60)
                        new_label(m,n) =13;
                    elseif (phi_d>=60) && (phi_d<120)
                        new_label(m,n) =14;
                    elseif (phi_d>=120) && (phi_d<=180)
                        new_label(m,n) =15;
                    end
                end
                
                
                % 6 part
                if ((theta_d>=300) && (theta_d<=360))
                    if (phi_d>=0) && (phi_d<60)
                        new_label(m,n) =16;
                    elseif (phi_d>=60) && (phi_d<120)
                        new_label(m,n) =17;
                    elseif (phi_d>=120) && (phi_d<=180)
                        new_label(m,n) =18;
                    end
                end
                
                
                
            end
        end
    end
    
    new_label_r = imresize(new_label, [360,480], 'nearest');
    
    normalLabel(i,:,:) = new_label_r;
end


%% save normal label to disk
for i = 1:N
    image = normalLabel(i,:,:);
    image = uint8(reshape(image, [360, 480]));
    
%     image_name = char(allNames(i));
    image_path = 'L:\Photogrammetry\Datasets\3dsemantics\noXYZ\noXYZ%2Farea_1_no_xyz\area_1\data\normal_label\conferenceRoom\2';
    full_image_name = fullfile(image_path, allFiles(i).name);
    
    imwrite(image, full_image_name);
end


%% normal label statistics
label_n = zeros(19, 1);
for i = 1:N
    for j = 1:size(normalLabel, 2)
       for k = 1:size(normalLabel, 3)
           class = normalLabel(i, j, k);
           if class == 0
               label_n(1,1) = label_n(1,1) + 1;
           elseif class == 1
               label_n(2,1) = label_n(2,1) + 1;
           elseif class == 2
               label_n(3,1) = label_n(3,1) + 1;
           elseif class == 3
               label_n(4,1) = label_n(4,1) + 1;
           elseif class == 4
               label_n(5,1) = label_n(5,1) + 1;
           elseif class == 5
               label_n(6,1) = label_n(6,1) + 1;
           elseif class == 6
               label_n(7,1) = label_n(7,1) + 1;
           elseif class == 7
               label_n(8,1) = label_n(8,1) + 1;
           elseif class == 8
               label_n(9,1) = label_n(9,1) + 1;
           elseif class == 9
               label_n(10,1) = label_n(10,1) + 1;
           elseif class == 10
               label_n(11,1) = label_n(11,1) + 1;
           elseif class == 11
               label_n(12,1) = label_n(12,1) + 1;
           elseif class == 12
               label_n(13,1) = label_n(13,1) + 1;
           elseif class == 13
               label_n(14,1) = label_n(14,1) + 1;
           elseif class == 14
               label_n(15,1) = label_n(15,1) + 1;
           elseif class == 15
               label_n(16,1) = label_n(16,1) + 1;
           elseif class == 16
               label_n(17,1) = label_n(17,1) + 1;
           elseif class == 17
               label_n(18,1) = label_n(18,1) + 1;
           elseif class == 18
               label_n(19,1) = label_n(19,1) + 1;
           end   
               
       end
    end
end

% percentage of every normal label
normal_percent = zeros(19, 1);
for q = 1:19
   normal_percent(q,1) = label_n(q, 1)/sum(label_n); 
end


% draw the figure of normal label distribution
bar(x, normal_percent);
xlabel('normal class');
ylabel('normal percent');
title('Normal Total Distribution');



normal_img = imread('camera_00d10d86db1e435081a837ced388375f_office_24_frame_0_domain_normals.png');
new_normal_img = zeros(1080, 1080, 3, 'double');


n = 0;
for i = 1:1080
    for j = 1:1080
        pixel = normal_img(i,j,:);
        pixel = double(reshape(pixel, [3,1]));
        
        if (pixel(1) == 128 && pixel(2) == 128 && pixel(3) == 128)
            n = n+1;
        end
        
        pixel_new = pixel - 127.5;
        
        r = sqrt(pixel_new(1)^2 + pixel_new(2)^2 + pixel_new(3)^2);
        
        x = pixel_new(1)/r;
        y = pixel_new(2)/r;
        z = pixel_new(3)/r;
        
        theta = atan(y/x);
        phi_d = acos(z);
        
        theta_d = round(rad2deg(theta));
        phi_d = round(rad2deg(phi_d));
        
        new_normal_img(i,j,1) = 1;
        new_normal_img(i,j,2) = theta_d;
        new_normal_img(i,j,3) = phi_d;
    end
end

d = 0;




