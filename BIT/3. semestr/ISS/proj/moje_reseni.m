%Prvni ukol
filename = 'xbrhel04.wav';
wavinfo = audioinfo(filename);
sampleperiod = wavinfo.SampleRate %vzorkovací frekvence
len = wavinfo.TotalSamples %počet vzorků
duration = wavinfo.Duration %délka v sekundách

%Drhý úkol
figure(1);
audio = audioread(filename);
transformation = fft(audio);
Fs = wavinfo.SampleRate;

spectrum = abs(transformation);
temp = spectrum(1:(len/2)+1);

func = Fs*(0:(len/2))/len;
plot(func,temp)
title('Spectrum');
xlabel('func (Hz)');
ylabel('|amplitude (func)|');

%Třetí úkol
[maxval, maxHz] = max(temp);
disp('Výskyt maximální hodnoty')
maxHz %zde se nachází maximální hodnota
disp('Maximální hodnota')
maxval %toto je maximální hodnota

%čtvrtý úkol
figure(2);
b=[0.2324 -0.4112 0.2324];
a=[1 0.2289 0.4662];
zplane(b,a);
p = roots(a);
if (isempty(p) | abs(p) < 1) 
  disp('Stabilni')
else
  disp('Nestabilni')
end

%pátý úkol
figure(3);
H = freqz(b,a,len);
func= (0:len-1)/len*Fs/2;
plot(func, abs(H));
xlabel('func');
ylabel('abs(H)');
grid;
disp('Jde o horní propust')

%šestý úkol
figure(4);
filtered = filter(b,a,audio); %vyfiltrujeme a zopakujeme postup z druhého úkolu
filtertransform = fft(filtered);
filterspec = abs(filtertransform);
temp2 = filterspec(1:(len/2)+1);
func = Fs*(0:(len/2))/len;
plot(func,temp2);
title('Spectrum');
xlabel('func (Hz)');
ylabel('|amplitude (func)|');

%sedmý úkol
[fmaxval, fmaxHz] = max(temp2);
disp('Výskyt maximální hodnoty filtrovaného signálu')
fmaxHz %zde se nachází maximální hodnota filtrovaného signálu
disp('Maximální hodnota filtrovaného signálu')
fmaxval %toto je maximální hodnota filtrovaného signálu

%osmý úkol
figure(5)
rectanglesign = [1 1 -1 -1];
rectanglesign = repmat(rectanglesign, 1, 80);
[correl, correllg] = xcorr(audio, rectanglesign);
[~,S] = max(abs(correl));
samples = correllg(S)+1;
display('Vozrky:');
display(samples);
display('Čas:');
display(samples/Fs);
scatter(samples+1:samples+80,audio(samples+1:samples+80));
grid;

%devátý úkol - bohužel jsem nepochopil, takže za něj nechci body, pomáhal mi kamarád
% figure(6); 
% len = length(audio);
% autocorrelk = zeros(0, 100, 'double');
% for i = -50:50
	% sum = 0;
	% for j = 1:len
		% if i+j > 0 && i+j < len+1
			% sum = sum + audio(j) * audio(i+j);
		% end
	% end
	% autocorrelk(i+51) = sum/len;
% end
% plot(-50:50, autocorrelk);