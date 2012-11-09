o2 = []
o3 = []

class Array
  def average
    av = 0
    self.each { |e| av += e }
    av /= self.length.to_f
  end
end

system "make parallel-test-O2"
system "make parallel-test-O3"
system "clear"

loop do

  t = Time.now
  `./bin/test-parallel-O2 10000`
  o2 << Time.now - t

  t = Time.now
  `./bin/test-parallel-O3 10000`
  o3 << Time.now - t

  printf "O2: %f    O3: %f     O3 / 02  %f        \r", o2.average, o3.average, o3.average.to_f / o2.average.to_f
end
