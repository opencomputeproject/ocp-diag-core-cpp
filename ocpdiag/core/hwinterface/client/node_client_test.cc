// Copyright 2022 Google LLC
//
// Use of this source code is governed by an MIT-style
// license that can be found in the LICENSE file or at
// https://opensource.org/licenses/MIT.

#include "ocpdiag/core/hwinterface/client/node_client.h"

#include <fstream>

#include "google/protobuf/duration.pb.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/flags/flag.h"
#include "absl/flags/reflection.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "ocpdiag/core/hwinterface/config.pb.h"
#include "ocpdiag/core/hwinterface/service.pb.h"
#include "ocpdiag/core/hwinterface/service_flags.h"
#include "ocpdiag/core/hwinterface/service_interface.h"
#include "ocpdiag/core/testing/file_utils.h"
#include "ocpdiag/core/testing/proto_matchers.h"
#include "ocpdiag/core/testing/status_matchers.h"

namespace ocpdiag::hwinterface {
namespace {

using ::ocpdiag::testing::EqualsProto;
using ::ocpdiag::testing::IsOk;
using ::ocpdiag::testing::IsOkAndHolds;
using ::ocpdiag::testing::StatusIs;
using ::ocpdiag::testutils::GetDataDependencyFilepath;
using ::testing::HasSubstr;
using ::testing::NotNull;
using ::testing::SizeIs;
using ::testing::StrEq;

class FakeClient : public OCPDiagServiceInterface {
 public:
  FakeClient(const EntityConfiguration& config) {}

  std::string Name() final { return "FakeClient"; }

  std::string GetBackendAddress() final { return "FakeClient Backend Address"; }

  absl::StatusOr<GetSensorsResponse> GetSensors(
      const GetSensorsRequest& req) override {
    return GetSensorsResponse();
  }
};

static absl::StatusOr<std::unique_ptr<OCPDiagServiceInterface>>
FakeCreateBackendFails(const EntityConfiguration& config) {
  return absl::UnavailableError("some bad return code");
}

static absl::StatusOr<std::unique_ptr<OCPDiagServiceInterface>>
FakeCreateBackendWorks(const EntityConfiguration& config) {
  return std::make_unique<FakeClient>(config);
}

TEST(NodeClientTest, CannotResolveDefaultBackendBuilder) {
  EntityConfiguration config;
  config.set_default_backend(BACKENDTYPE_GSYS);
  absl::flat_hash_map<BackendType, NodeClient::BuilderType> lookup;
  auto client = NodeClient::Create(config, lookup);
  EXPECT_THAT(client, StatusIs(absl::StatusCode::kNotFound));
}

TEST(NodeClientTest, CannotResolveEnumeratedBackendBuilder) {
  EntityConfiguration config;
  config.set_default_backend(BACKENDTYPE_GSYS);
  (*config.mutable_method_configurations())[BACKENDMETHOD_GET_SENSORS] =
      BACKENDTYPE_MACHINE_MANAGER;
  absl::flat_hash_map<BackendType, NodeClient::BuilderType> lookup{
      {BACKENDTYPE_GSYS, &FakeCreateBackendWorks}};
  auto client = NodeClient::Create(config, lookup);
  EXPECT_THAT(client, StatusIs(absl::StatusCode::kNotFound));
}

TEST(NodeClientTest, DefaultBackendCreationFailure) {
  EntityConfiguration config;
  config.set_default_backend(BACKENDTYPE_GSYS);
  absl::flat_hash_map<BackendType, NodeClient::BuilderType> lookup{
      {BACKENDTYPE_GSYS, &FakeCreateBackendFails}};
  auto client = NodeClient::Create(config, lookup);
  EXPECT_THAT(client, StatusIs(absl::StatusCode::kUnavailable));
}

TEST(NodeClientTest, EnumeratedBackendCreationFailure) {
  EntityConfiguration config;
  config.set_default_backend(BACKENDTYPE_GSYS);
  (*config.mutable_method_configurations())[BACKENDMETHOD_GET_SENSORS] =
      BACKENDTYPE_MACHINE_MANAGER;

  absl::flat_hash_map<BackendType, NodeClient::BuilderType> lookup{
      {BACKENDTYPE_GSYS, &FakeCreateBackendWorks},
      {BACKENDTYPE_MACHINE_MANAGER, &FakeCreateBackendFails},
  };
  auto client = NodeClient::Create(config, lookup);
  EXPECT_THAT(client, StatusIs(absl::StatusCode::kUnavailable));
}

TEST(NodeClientTest, ClientCreationSucceedsWithJustDefault) {
  EntityConfiguration config;
  config.set_default_backend(BACKENDTYPE_GSYS);

  absl::flat_hash_map<BackendType, NodeClient::BuilderType> lookup{
      {BACKENDTYPE_GSYS, &FakeCreateBackendWorks},
      {BACKENDTYPE_MACHINE_MANAGER, &FakeCreateBackendFails},
  };
  auto client = NodeClient::Create(config, lookup);
  EXPECT_THAT(client, IsOk());
}

TEST(NodeClientTest, ClientCreationSucceedsWithMultipleBackends) {
  EntityConfiguration config;
  config.set_default_backend(BACKENDTYPE_GSYS);
  (*config.mutable_method_configurations())[BACKENDMETHOD_GET_SENSORS] =
      BACKENDTYPE_MACHINE_MANAGER;

  absl::flat_hash_map<BackendType, NodeClient::BuilderType> lookup{
      {BACKENDTYPE_GSYS, &FakeCreateBackendWorks},
      {BACKENDTYPE_MACHINE_MANAGER, &FakeCreateBackendWorks},
  };
  auto client = NodeClient::Create(config, lookup);
  EXPECT_THAT(client, IsOk());
}

class NodeClientApiTest : public ::testing::Test {
 protected:
  void SetUp() override {
    EntityConfiguration config;
    config.set_default_backend(BACKENDTYPE_GSYS);
    absl::flat_hash_map<BackendType, NodeClient::BuilderType> lookup{
        {BACKENDTYPE_GSYS, &FakeCreateBackendWorks},
    };
    absl::StatusOr<std::unique_ptr<NodeClient>> c =
        NodeClient::Create(config, lookup);
    ASSERT_OK(c);
    client_ = std::move(c.value());
  }

  std::unique_ptr<NodeClient> client_;
};

TEST_F(NodeClientApiTest, GetCpuInfo) {
  absl::StatusOr<GetCpuInfoResponse> resp =
      client_->GetCpuInfo(GetCpuInfoRequest{});
  EXPECT_EQ(resp.status().code(), absl::StatusCode::kUnimplemented);
  EXPECT_THAT(resp.status().message(), HasSubstr("FakeClient"));
}

TEST_F(NodeClientApiTest, GetSensors) {
  absl::StatusOr<GetSensorsResponse> resp =
      client_->GetSensors(GetSensorsRequest{});
  EXPECT_THAT(resp, IsOk());
}

TEST(NodeClientDefaultResolver, BasicTest) {
  auto resolver = internal::DefaultBuilderResolver();
  EXPECT_NE(resolver.find(BACKENDTYPE_HOST), resolver.end());
}

TEST(LoadConfigTest, DefaultBackendOverride) {
  // If both config file and input flag didn't set hwinterface default backend,
  // backend will be set to gsys.

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
  EntityConfiguration* config = configs->mutable_node_entity_configurations(0);

  EXPECT_EQ(config->default_backend(), BACKENDTYPE_HOST);

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  EXPECT_EQ(config_single_node->default_backend(), BACKENDTYPE_HOST);
}

TEST(LoadConfigTest, EmptyBackendOverride) {
  // If config file didn't set hwinterface default backend and backend flag is
  // set to unknown, backend will be set to gsys.
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_UNKNOWN);

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
  EntityConfiguration* config = configs->mutable_node_entity_configurations(0);

  EXPECT_EQ(config->default_backend(), BACKENDTYPE_HOST);

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  EXPECT_EQ(config_single_node->default_backend(), BACKENDTYPE_HOST);
}

TEST(LoadConfigTest, SingleNodeBackendRedfishConfigFile) {
  absl::SetFlag(
      &FLAGS_hwinterface_config_file_path,
      GetDataDependencyFilepath(
          "ocpdiag/core/hwinterface/client/testdata/"
          "redfish-single-node-hwinterface.config"));
  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
  EntityConfiguration* config = configs->mutable_node_entity_configurations(0);

  EXPECT_EQ(config->default_backend(), BACKENDTYPE_REDFISH);
  const ocpdiag::hwinterface::RedfishConfigOptions& redfish_options =
      config->config_options().redfish_options();
  EXPECT_EQ(redfish_options.auth_method(),
            RedfishConfigOptions_AuthMethod_BASIC);
  EXPECT_EQ(redfish_options.basic_auth_config().user_name(), "root");
  EXPECT_EQ(redfish_options.basic_auth_config().password(), "redfish");
}

TEST(LoadConfigTest, SingleNodeBackendOverride) {
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_GSYS);

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
  EntityConfiguration* config = configs->mutable_node_entity_configurations(0);

  EXPECT_EQ(config->default_backend(), BACKENDTYPE_GSYS);

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  EXPECT_EQ(config_single_node->default_backend(), BACKENDTYPE_GSYS);
}

TEST(LoadConfigTest, SingleNodeTargetAddressOverride) {
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_GSYS);
  absl::SetFlag(&FLAGS_hwinterface_target_address, "localhost");

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
  EntityConfiguration* config = configs->mutable_node_entity_configurations(0);

  EXPECT_THAT(config->entity().host_address(), StrEq("localhost"));

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  EXPECT_THAT(config_single_node->entity().host_address(), StrEq("localhost"));
}

TEST(LoadConfigTest, MultiNodeOverrideOOBOverride) {
  absl::FlagSaver fs;
  std::vector<NodeOverride> overrides;
  absl::ParseFlag("redfish://abcd1-bmc:443", &overrides, nullptr);
  absl::SetFlag(&FLAGS_mhi_config, overrides);
  // The following overrides should be ignored.
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_GSYS);
  absl::SetFlag(&FLAGS_hwinterface_target_address, "localhost");

  // Expected configs.
  EntityConfigurations expected_configs;
  EntityConfiguration* expected_config =
      expected_configs.add_node_entity_configurations();
  expected_config->set_default_backend(BACKENDTYPE_REDFISH);
  expected_config->mutable_config_options()
      ->mutable_redfish_options()
      ->set_target_address("abcd1-bmc");
  expected_config->mutable_config_options()
      ->mutable_redfish_options()
      ->set_port(443);

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  EXPECT_THAT(*configs, EqualsProto(expected_configs));

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  EXPECT_THAT(*config_single_node, EqualsProto(*expected_config));
}

TEST(LoadConfigTest, MultiNodeOverrideOOBOverrideIPv6) {
  absl::FlagSaver fs;
  std::vector<NodeOverride> overrides;
  absl::ParseFlag("redfish://[1050:0:0:0:5:600:300c:326b]:443", &overrides,
                  nullptr);
  absl::SetFlag(&FLAGS_mhi_config, overrides);
  // The following overrides should be ignored.
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_GSYS);
  absl::SetFlag(&FLAGS_hwinterface_target_address, "localhost");

  // Expected configs.
  EntityConfigurations expected_configs;
  EntityConfiguration* expected_config =
      expected_configs.add_node_entity_configurations();
  expected_config->set_default_backend(BACKENDTYPE_REDFISH);
  expected_config->mutable_config_options()
      ->mutable_redfish_options()
      ->set_target_address("[1050:0:0:0:5:600:300c:326b]");
  expected_config->mutable_config_options()
      ->mutable_redfish_options()
      ->set_port(443);

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  EXPECT_THAT(*configs, EqualsProto(expected_configs));

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  EXPECT_THAT(*config_single_node, EqualsProto(*expected_config));
}

TEST(LoadConfigTest, MultiNodeOverrideIBOverride) {
  absl::FlagSaver fs;
  std::vector<NodeOverride> overrides;
  absl::ParseFlag("gsys://abcd1:3995", &overrides, nullptr);
  absl::SetFlag(&FLAGS_mhi_config, overrides);
  // The following overrides should be ignored.
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_REDFISH);
  absl::SetFlag(&FLAGS_hwinterface_target_address, "abcd1-bmc");

  // Expected configs.
  EntityConfigurations expected_configs;
  EntityConfiguration* expected_config =
      expected_configs.add_node_entity_configurations();
  expected_config->set_default_backend(BACKENDTYPE_GSYS);
  expected_config->mutable_entity()->set_host_address("abcd1");
  // port is ignored.

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  EXPECT_THAT(*configs, EqualsProto(expected_configs));

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  EXPECT_THAT(*config_single_node, EqualsProto(*expected_config));
}

TEST(LoadConfigTest, MultiNodeOverrides) {
  std::vector<NodeOverride> overrides;
  absl::ParseFlag(
      "gsys://abcd1:3995,redfish://abcd1-bmc:443:tls,redfish://abcd1-bmc1:80",
      &overrides, nullptr);
  absl::SetFlag(&FLAGS_mhi_config, overrides);

  // Expected configs.
  EntityConfigurations expected_configs;
  EntityConfiguration* expected_config1 =
      expected_configs.add_node_entity_configurations();
  expected_config1->set_default_backend(BACKENDTYPE_GSYS);
  expected_config1->mutable_entity()->set_host_address("abcd1");
  // gsys backend port is ignored.

  EntityConfiguration* expected_config2 =
      expected_configs.add_node_entity_configurations();
  expected_config2->set_default_backend(BACKENDTYPE_REDFISH);
  expected_config2->mutable_config_options()
      ->mutable_redfish_options()
      ->set_target_address("abcd1-bmc");
  expected_config2->mutable_config_options()
      ->mutable_redfish_options()
      ->set_port(443);
  expected_config2->mutable_config_options()
      ->mutable_redfish_options()
      ->set_auth_method(RedfishConfigOptions_AuthMethod_TLS);

  EntityConfiguration* expected_config3 =
      expected_configs.add_node_entity_configurations();
  expected_config3->set_default_backend(BACKENDTYPE_REDFISH);
  expected_config3->mutable_config_options()
      ->mutable_redfish_options()
      ->set_target_address("abcd1-bmc1");
  expected_config3->mutable_config_options()
      ->mutable_redfish_options()
      ->set_port(80);

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  EXPECT_THAT(*configs, EqualsProto(expected_configs));

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  EXPECT_THAT(config_single_node, StatusIs(absl::StatusCode::kInternal));
}

TEST(LoadConfigTest, MultiNodeOverridesWithDefaultFile) {
  absl::FlagSaver fs;
  // Create mhi_config file in test directory.
  std::string config_filepath =
      absl::StrCat(getenv("TEST_TMPDIR"), "/mhi_config.json");
  std::ofstream src;
  src.open(config_filepath);
  src << R"json({
    "node_entity_configurations": [
      {
        "entity": { "host_address": "abcd1" },
        "config_options": { "redfish_options": { "target_address": "abcd1-bmc" } }
      },
      { "entity": {"host_address": "abcd2"}}
    ]
  })json";
  src.close();

  //
  absl::SetFlag(&FLAGS_hwinterface_config_file_path, config_filepath);
  std::vector<NodeOverride> overrides;
  absl::ParseFlag("gsys://abcd1:3995,redfish://abcd1-bmc:443,gsys://abcd2:3997",
                  &overrides, nullptr);
  absl::SetFlag(&FLAGS_mhi_config, overrides);

  // Expected configs.
  EntityConfigurations expected_configs;
  EntityConfiguration* expected_config1 =
      expected_configs.add_node_entity_configurations();
  // Chooses the last backend overriden.
  expected_config1->set_default_backend(BACKENDTYPE_REDFISH);
  expected_config1->mutable_entity()->set_host_address("abcd1");
  expected_config1->mutable_config_options()
      ->mutable_redfish_options()
      ->set_target_address("abcd1-bmc");
  expected_config1->mutable_config_options()
      ->mutable_redfish_options()
      ->set_port(443);

  EntityConfiguration* expected_config2 =
      expected_configs.add_node_entity_configurations();
  expected_config2->set_default_backend(BACKENDTYPE_GSYS);
  expected_config2->mutable_entity()->set_host_address("abcd2");

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  EXPECT_THAT(*configs, EqualsProto(expected_configs));

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  EXPECT_THAT(config_single_node, StatusIs(absl::StatusCode::kInternal));
}

TEST(LoadConfigTest, RedfishAuthMethodOverride) {
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_REDFISH);
  absl::SetFlag(&FLAGS_hwinterface_redfish_auth_method,
                RedfishConfigOptions_AuthMethod::
                    RedfishConfigOptions_AuthMethod_TLS);

  // Test LoadConfigs.
  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
  const EntityConfiguration& config = configs->node_entity_configurations(0);

  EXPECT_EQ(config.config_options().redfish_options().auth_method(),
            RedfishConfigOptions_AuthMethod_TLS);

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  EXPECT_EQ(config.config_options().redfish_options().auth_method(),
            RedfishConfigOptions_AuthMethod_TLS);
}

// Test no cache policy is set when overrides are not specified.
TEST(LoadConfigTest, RedfishCachePolicyNoOverride) {
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_REDFISH);

  // Test no cache
  {
    RedfishConfigOptions::RedfishCachePolicy overrides;

    absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
    ASSERT_OK(configs);
    ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
    const EntityConfiguration& config = configs->node_entity_configurations(0);

    EXPECT_FALSE(
        config.config_options().redfish_options().has_redfish_cache_policy());

    absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
    ASSERT_OK(config_single_node);
    EXPECT_FALSE(config_single_node->config_options()
                     .redfish_options()
                     .has_redfish_cache_policy());
  }

  // Test empty cache policy argument
  {
    RedfishConfigOptions::RedfishCachePolicy overrides;
    absl::ParseFlag("", &overrides, nullptr);
    absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
    ASSERT_OK(configs);
    ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
    const EntityConfiguration& config = configs->node_entity_configurations(0);

    EXPECT_FALSE(
        config.config_options().redfish_options().has_redfish_cache_policy());

    absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
    ASSERT_OK(config_single_node);
    EXPECT_FALSE(config_single_node->config_options()
                     .redfish_options()
                     .has_redfish_cache_policy());
  }
}

TEST(LoadConfigTest, RedfishCachePolicyOverrideTimeBased) {
  RedfishConfigOptions::RedfishCachePolicy overrides;
  absl::ParseFlag("time_based_cache:1000", &overrides, nullptr);
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_REDFISH);
  absl::SetFlag(&FLAGS_hwinterface_redfish_cache_policy, overrides);

  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
  const EntityConfiguration& config = configs->node_entity_configurations(0);

  ASSERT_TRUE(
      config.config_options().redfish_options().has_redfish_cache_policy());
  ASSERT_TRUE(config.config_options()
                  .redfish_options()
                  .redfish_cache_policy()
                  .has_time_based_cache());
  EXPECT_EQ(config.config_options()
                .redfish_options()
                .redfish_cache_policy()
                .time_based_cache()
                .seconds(),
            1000);

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  ASSERT_TRUE(config_single_node->config_options()
                  .redfish_options()
                  .has_redfish_cache_policy());
  ASSERT_TRUE(config_single_node->config_options()
                  .redfish_options()
                  .redfish_cache_policy()
                  .has_time_based_cache());
  EXPECT_EQ(config_single_node->config_options()
                .redfish_options()
                .redfish_cache_policy()
                .time_based_cache()
                .seconds(),
            1000);
}

TEST(LoadConfigTest, RedfishCachePolicyOverrideNoCache) {
  RedfishConfigOptions::RedfishCachePolicy overrides;
  absl::ParseFlag("no_cache", &overrides, nullptr);
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_REDFISH);
  absl::SetFlag(&FLAGS_hwinterface_redfish_cache_policy, overrides);

  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
  const EntityConfiguration& config = configs->node_entity_configurations(0);

  ASSERT_TRUE(
      config.config_options().redfish_options().has_redfish_cache_policy());
  EXPECT_TRUE(config.config_options()
                  .redfish_options()
                  .redfish_cache_policy()
                  .has_no_cache());

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  ASSERT_TRUE(config_single_node->config_options()
                  .redfish_options()
                  .has_redfish_cache_policy());
  EXPECT_TRUE(config_single_node->config_options()
                  .redfish_options()
                  .redfish_cache_policy()
                  .has_no_cache());
}

TEST(LoadConfigTest, RedfishQueryOverride) {
  RedfishConfigOptions::RedfishQueryOverrides overrides;
  absl::ParseFlag("sensor_query:mock_query_id", &overrides, nullptr);
  absl::SetFlag(&FLAGS_hwinterface_redfish_query_overrides, overrides);
  absl::SetFlag(&FLAGS_hwinterface_default_backend, BACKENDTYPE_REDFISH);

  absl::StatusOr<EntityConfigurations> configs = LoadConfigs();
  ASSERT_OK(configs);
  ASSERT_THAT(configs->node_entity_configurations(), SizeIs(1));
  const EntityConfiguration& config = configs->node_entity_configurations(0);

  EXPECT_EQ(config.config_options()
                .redfish_options()
                .redfish_query_overrides()
                .sensor_query(),
            "mock_query_id");

  // Test LoadConfig.
  absl::StatusOr<EntityConfiguration> config_single_node = LoadConfig();
  ASSERT_OK(config_single_node);
  EXPECT_EQ(config_single_node->config_options()
                .redfish_options()
                .redfish_query_overrides()
                .sensor_query(),
            "mock_query_id");
}

}  // namespace
}  // namespace ocpdiag::hwinterface
