filename = 'H:/src/S4Mods/BoneEnhancer/Data/BP_Lumbar_SingleSlice_Double.mha';
[BoneProbability,info]=ReadData3D(filename);
BoneProbability = imrotate(BoneProbability, -90);
%imshow(BP)

E_int = 1 - BoneProbability;

minC = zeros(size(BoneProbability));
minI = zeros(size(BoneProbability));

for i=1:size(BoneProbability,1)
    for j=1:size(BoneProbability,2)
        for k=1:size(BoneProbability,2)
               
        end                    
        minC(i,j) = E_int(i,j) + 111;
    end    
end