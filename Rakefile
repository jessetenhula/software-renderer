task :build do
  sh 'gcc src/*.c -o bin/renderer -I include -std=c99'
end

task :run, [:out_file] do |task, args|
  sh "./bin/renderer #{args[:out_file]}"
end
