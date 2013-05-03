tc = 0
pt = 0
tc2 = 0
pt2 = 0
tc3 = 0
pt3 = 0

if ARGV.length != 5
  puts "#{$0} <times> <num citys> <generation limit> "
  puts "<num generations> <num threads>"
  exit
end

system "make tsp-test"
system "make tsp-test-O2"
system "make tsp-test-O3"
system "make tsp_pthread-test"
system "make tsp_pthread-test-O2"
system "make tsp_pthread-test-O3"

ARGV[0].to_i.times do |i|
  puts "#{ARGV[0].to_i - i}"

  t = Time.now
  system "./bin/tsp #{ARGV[1]} #{ARGV[2]} #{ARGV[3]} #{ARGV[4]} 0"
  tc += Time.now - t

  t = Time.now
  system "./bin/tsp_pthread #{ARGV[1]} #{ARGV[2]} #{ARGV[3]} #{ARGV[4]} 0"
  pt += Time.now - t


  t = Time.now
  system "./bin/tsp-O2 #{ARGV[1]} #{ARGV[2]} #{ARGV[3]} #{ARGV[4]} 0"
  tc2 += Time.now - t

  t = Time.now
  system "./bin/tsp_pthread-O2 #{ARGV[1]} #{ARGV[2]} #{ARGV[3]} #{ARGV[4]} 0"
  pt2 += Time.now - t


  t = Time.now
  system "./bin/tsp-O3 #{ARGV[1]} #{ARGV[2]} #{ARGV[3]} #{ARGV[4]} 0"
  tc3 += Time.now - t

  t = Time.now
  system "./bin/tsp_pthread-O3 #{ARGV[1]} #{ARGV[2]} #{ARGV[3]} #{ARGV[4]} 0"
  pt3 += Time.now - t
end


puts "Thread Client O0: #{tc  /  ARGV[0].to_i}"
puts "Thread Client O2: #{tc2 /  ARGV[0].to_i}"
puts "Thread Client O3: #{tc3 /  ARGV[0].to_i}"
puts "PThreads O0:      #{pt  /  ARGV[0].to_i}"
puts "PThreads O2:      #{pt2 /  ARGV[0].to_i}"
puts "PThreads O3:      #{pt3 /  ARGV[0].to_i}"
