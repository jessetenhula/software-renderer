task :build do
  sh 'gcc src/*.c -o bin/renderer -I include -std=c99'
end

task :run => :build do
  sh './bin/renderer.exe out.tga'
end
