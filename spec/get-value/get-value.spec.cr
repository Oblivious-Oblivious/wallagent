describe Wallagent::GetValue do
  it "returns Hello, World!" do
    Wallagent::GetValue.new.value.should eq "Hello, World!";
  end
end
