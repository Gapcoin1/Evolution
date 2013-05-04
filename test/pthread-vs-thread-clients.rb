tc = 0
pt = 0
tc2 = 0
pt2 = 0
tc3 = 0
pt3 = 0

_1tc = 0
_1pt = 0
_1tc2 = 0
_1pt2 = 0
_1tc3 = 0
_1pt3 = 0


less_tc = 0
less_pt = 0
less_tc2 = 0
less_pt2 = 0
less_tc3 = 0
less_pt3 = 0

less__1tc = 0
less__1pt = 0
less__1tc2 = 0
less__1pt2 = 0
less__1tc3 = 0
less__1pt3 = 0

if ARGV.length != 3
  puts "#{$0} <times> <num generations> <num threads>"
  exit
end

system "make parallel-test"
system "make parallel-test-O2"
system "make parallel-test-O3"
system "make parallel_pthread-test"
system "make parallel_pthread-test-O2"
system "make parallel_pthread-test-O3"

system "make parallel_less_sync-test"
system "make parallel_less_sync-test-O2"
system "make parallel_less_sync-test-O3"
system "make parallel_pthread_less_sync-test"
system "make parallel_pthread_less_sync-test-O2"
system "make parallel_pthread_less_sync-test-O3"

ARGV[0].to_i.times do |i|
  puts "#{ARGV[0].to_i - i}"

  t = Time.now
  system "./bin/test-parallel #{ARGV[1]} #{ARGV[2]} 0"
  tc += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread #{ARGV[1]} #{ARGV[2]} 0"
  pt += Time.now - t


  t = Time.now
  system "./bin/test-parallel-O2 #{ARGV[1]} #{ARGV[2]} 0"
  tc2 += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread-O2 #{ARGV[1]} #{ARGV[2]} 0"
  pt2 += Time.now - t


  t = Time.now
  system "./bin/test-parallel-O3 #{ARGV[1]} #{ARGV[2]} 0"
  tc3 += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread-O3 #{ARGV[1]} #{ARGV[2]} 0"
  pt3 += Time.now - t


  t = Time.now
  system "./bin/test-parallel #{ARGV[1]} 1 0"
  _1tc += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread #{ARGV[1]} 1 0"
  _1pt += Time.now - t


  t = Time.now
  system "./bin/test-parallel-O2 #{ARGV[1]} 1 0"
  _1tc2 += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread-O2 #{ARGV[1]} 1 0"
  _1pt2 += Time.now - t


  t = Time.now
  system "./bin/test-parallel-O3 #{ARGV[1]} 1 0"
  _1tc3 += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread-O3 #{ARGV[1]} 1 0"
  _1pt3 += Time.now - t



  t = Time.now
  system "./bin/test-parallel_less_sync #{ARGV[1]} #{ARGV[2]} 0"
  less_tc += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread_less_sync #{ARGV[1]} #{ARGV[2]} 0"
  less_pt += Time.now - t


  t = Time.now
  system "./bin/test-parallel_less_sync-O2 #{ARGV[1]} #{ARGV[2]} 0"
  less_tc2 += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread_less_sync-O2 #{ARGV[1]} #{ARGV[2]} 0"
  less_pt2 += Time.now - t


  t = Time.now
  system "./bin/test-parallel_less_sync-O3 #{ARGV[1]} #{ARGV[2]} 0"
  less_tc3 += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread_less_sync-O3 #{ARGV[1]} #{ARGV[2]} 0"
  less_pt3 += Time.now - t


  t = Time.now
  system "./bin/test-parallel_less_sync #{ARGV[1]} 1 0"
  less__1tc += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread_less_sync #{ARGV[1]} 1 0"
  less__1pt += Time.now - t


  t = Time.now
  system "./bin/test-parallel_less_sync-O2 #{ARGV[1]} 1 0"
  less__1tc2 += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread_less_sync-O2 #{ARGV[1]} 1 0"
  less__1pt2 += Time.now - t


  t = Time.now
  system "./bin/test-parallel_less_sync-O3 #{ARGV[1]} 1 0"
  less__1tc3 += Time.now - t

  t = Time.now
  system "./bin/test-parallel_pthread_less_sync-O3 #{ARGV[1]} 1 0"
  less__1pt3 += Time.now - t

end


puts "[#{ARGV[2]}]Thread Client O0: #{tc  /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]Thread Client O2: #{tc2 /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]Thread Client O3: #{tc3 /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]PThreads O0:      #{pt  /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]PThreads O2:      #{pt2 /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]PThreads O3:      #{pt3 /  ARGV[0].to_i}"
puts "[1]Thread Client O0: #{_1tc  /  ARGV[0].to_i}"
puts "[1]Thread Client O2: #{_1tc2 /  ARGV[0].to_i}"
puts "[1]Thread Client O3: #{_1tc3 /  ARGV[0].to_i}"
puts "[1]PThreads O0:      #{_1pt  /  ARGV[0].to_i}"
puts "[1]PThreads O2:      #{_1pt2 /  ARGV[0].to_i}"
puts "[1]PThreads O3:      #{_1pt3 /  ARGV[0].to_i}"

puts "\nLess Sync:"
puts "[#{ARGV[2]}]Thread Client O0: #{less_tc  /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]Thread Client O2: #{less_tc2 /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]Thread Client O3: #{less_tc3 /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]PThreads O0:      #{less_pt  /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]PThreads O2:      #{less_pt2 /  ARGV[0].to_i}"
puts "[#{ARGV[2]}]PThreads O3:      #{less_pt3 /  ARGV[0].to_i}"
puts "[1]Thread Client O0: #{less__1tc  /  ARGV[0].to_i}"
puts "[1]Thread Client O2: #{less__1tc2 /  ARGV[0].to_i}"
puts "[1]Thread Client O3: #{less__1tc3 /  ARGV[0].to_i}"
puts "[1]PThreads O0:      #{less__1pt  /  ARGV[0].to_i}"
puts "[1]PThreads O2:      #{less__1pt2 /  ARGV[0].to_i}"
puts "[1]PThreads O3:      #{less__1pt3 /  ARGV[0].to_i}"
